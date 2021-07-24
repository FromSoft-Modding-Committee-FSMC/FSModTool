#include "modelglview.h"
#include <iostream>
#include <utility>
#include <QMouseEvent>
#include <QWheelEvent>

void ModelGLView::setModel(std::shared_ptr<Model> model_)
{    
    model = std::move(model_);
}

void ModelGLView::initializeGL()
{
    if (glFuncs == nullptr)
        glFuncs = context()->extraFunctions();
    
    //Initialize some basic OGL stuff
    glFuncs->glClearColor(0x34 / 255.0f, 0x49 / 255.0f, 0x5E / 255.0f, 1.f);    //Nice Palettes: https://htmlcolorcodes.com/

    //Set some OGL render states (cull etc)
    glFuncs->glEnable(GL_CULL_FACE);
    glFuncs->glEnable(GL_DEPTH_TEST);

    BuildGrid();

    while (model == nullptr) { /* Wait for model */ };
    if(model->animations.empty())
        BuildTMDModel();
    else
    {
        BuildMOAnimation();
        setCurAnim(0);
    }
}

void ModelGLView::mouseMoveEvent(QMouseEvent * event)
{
    //Rotation
    if (event->buttons() == Qt::MiddleButton)
    {
        if (lastMousePos.x() != -999)
        {
            auto mousePosDiff = event->globalPos() - lastMousePos;
            camRotY += (mousePosDiff.x() * 0.0174533f) * 0.25f;
            camRotZ += (mousePosDiff.y() * 0.0174533f) * 0.25f;
        }
    }
    else if (event->buttons() == Qt::RightButton)
    {
        if (lastMousePos.x() != -999)
        {
            auto mousePosDiff = event->globalPos() - lastMousePos;
            camTranslateX += -mousePosDiff.x();
            camTranslateY += mousePosDiff.y();
        }
    }
    lastMousePos = event->globalPos();
}

void ModelGLView::wheelEvent(QWheelEvent *event)
{
    camZoom -= (event->angleDelta().y() / 120.f) * 8.f;
    camZoom = qMax(camZoom, 8.f);
}

void ModelGLView::mouseReleaseEvent(QMouseEvent * event)
{
    Q_UNUSED(event)
    lastMousePos = {-999, -999};
}

void ModelGLView::resizeGL(int w, int h)
{
    //Build Projection Matrix according to new w & h
    glMatProj.setToIdentity();
    glMatProj.perspective(pFoV, static_cast<float>(w) / static_cast<float>(h), zNear, zFar);
}

void ModelGLView::paintGL()
{
    //Clear Buffers
    glFuncs->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Stop camera flipping :)
    camRotZ = qMin(qMax(-1.570750f, camRotZ), 1.570750f);

    //Apply Rotation to glCamFrom
    glCamFrom.setX(std::cos(camRotY) * std::cos(camRotZ) * camZoom);
    glCamFrom.setY(std::sin(camRotZ) * camZoom);
    glCamFrom.setZ(std::sin(camRotY) * std::cos(camRotZ) * camZoom);

    //Build View Matrix
    glMatView.setToIdentity();
    glMatView.lookAt(glCamFrom, glCamTo, glCamUp);
    glMatView.translate(std::sin(camRotY) * camTranslateX , camTranslateY, std::cos(camRotY) * camTranslateX);

    //Build World Matrix
    glMatWorld.setToIdentity();
    glMatWorld.scale(glModelScale);
    glMatWorld.rotate(glModelRotation.x(), vecLeft);
    glMatWorld.rotate(glModelRotation.y(), vecUp);
    glMatWorld.rotate(glModelRotation.z(), vecFront);
    
    DrawGrid();

    //If curAnim is -1, we must be drawing a TMD, else it's MO
    if(curAnim == -1)
    {
        DrawTMDModel();
    }else{
        DrawMOAnimation();
    }
}

//
// MO Build & Draw
//
void ModelGLView::BuildMOAnimation()
{
    //Clear GLMesh if a new TMD is built
    glFuncs->glBindVertexArray(0);
    glFuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);

    for(GLMesh &mesh : meshes)
    {
        glFuncs->glDeleteBuffers(mesh.frames.size(), mesh.frames.data());
        glFuncs->glDeleteVertexArrays(1, &mesh.VAO);
    }
    meshes.clear();

    for (const auto& Anim : model->animations)
    {
        //Build each animation frame
        unsigned int i = 0;

        GLMesh& glMesh = meshes.emplace_back();
        glMesh.frames.resize(Anim.frameIndexes.size());

        //Begin Generating OpenGL stuff for this frame
        glFuncs->glGenBuffers(Anim.frameIndexes.size(), glMesh.frames.data());
        glFuncs->glGenVertexArrays(1, &glMesh.VAO);

        //Bind array buffer
        glFuncs->glBindVertexArray(glMesh.VAO);

        for (const size_t frameID : Anim.frameIndexes)
        {
            const Model::Mesh& frame1 = model->morphTargets[model->animFrames[frameID].frameID];
            const Model::Mesh& frame2
                = i == Anim.frameIndexes.size() - 1
                      ? model->morphTargets[model->animFrames[Anim.frameIndexes[0]].frameID]
                      : model->morphTargets[model->animFrames[frameID + 1].frameID];

            //Now build the actual frame!
            std::vector<MOVertex> vertices;

            for (const Model::Primitive& prim : model->baseObjects[0].primitives)
            {
                if (prim.isTriangle())
                {
                    MOVertex v0, v1, v2;

                    //Vertex 1
                    v0.position1 = frame1.vertices[prim.vertex0];
                    v0.position2 = frame2.vertices[prim.vertex0];
                    v0.normal = frame1.normals[prim.normal0];
                    v0.colour = prim.Colour0();
                    v0.texcoord = {prim.u0 / 255.f, prim.v0 / 255.f};

                    //Vertex 2
                    v1.position1 = frame1.vertices[prim.vertex1];
                    v1.position2 = frame2.vertices[prim.vertex1];
                    v1.normal = prim.isSmooth() ? frame1.normals[prim.normal1]
                                                : frame1.normals[prim.normal0];
                    v1.colour = prim.isGradation() ? prim.Colour1() : prim.Colour0();
                    v1.texcoord = {prim.u1 / 255.f, prim.v1 / 255.f};

                    //Vertex 3
                    v2.position1 = frame1.vertices[prim.vertex2];
                    v2.position2 = frame2.vertices[prim.vertex2];
                    v2.normal = prim.isSmooth() ? frame1.normals[prim.normal2]
                                                : frame1.normals[prim.normal0];
                    v2.colour = prim.isGradation() ? prim.Colour2() : prim.Colour0();
                    v2.texcoord = {prim.u2 / 255.f, prim.v2 / 255.f};

                    vertices.push_back(v2);
                    vertices.push_back(v1);
                    vertices.push_back(v0);

                } else if (prim.isQuad())
                {
                    MOVertex v0, v1, v2, v3;

                    //Vertex 1
                    v0.position1 = frame1.vertices[prim.vertex0];
                    v0.position2 = frame2.vertices[prim.vertex0];
                    v0.normal = frame1.normals[prim.normal0];
                    v0.colour = prim.Colour0();
                    v0.texcoord = {prim.u0 / 255.f, prim.v0 / 255.f};

                    //Vertex 2
                    v1.position1 = frame1.vertices[prim.vertex1];
                    v1.position2 = frame2.vertices[prim.vertex1];
                    v1.normal = prim.isSmooth() ? frame1.normals[prim.normal1]
                                                : frame1.normals[prim.normal0];
                    v1.colour = prim.isGradation() ? prim.Colour1() : prim.Colour0();
                    v1.texcoord = {prim.u1 / 255.f, prim.v1 / 255.f};

                    //Vertex 3
                    v2.position1 = frame1.vertices[prim.vertex2];
                    v2.position2 = frame2.vertices[prim.vertex2];
                    v2.normal = prim.isSmooth() ? frame1.normals[prim.normal2]
                                                : frame1.normals[prim.normal0];
                    v2.colour = prim.isGradation() ? prim.Colour2() : prim.Colour0();
                    v2.texcoord = {prim.u2 / 255.f, prim.v2 / 255.f};

                    //Vertex 4
                    v3.position1 = frame1.vertices[prim.vertex3];
                    v3.position2 = frame2.vertices[prim.vertex3];
                    v3.normal = prim.isSmooth() ? frame1.normals[prim.normal3]
                                                : frame1.normals[prim.normal0];
                    v3.colour = prim.isGradation() ? prim.Colour3() : prim.Colour0();
                    v3.texcoord = {prim.u3 / 255.f, prim.v3 / 255.f};

                    //Tri 1 (2, 1, 0)
                    vertices.push_back(v2);
                    vertices.push_back(v1);
                    vertices.push_back(v0);

                    //Tri 2 (2, 3, 1)
                    vertices.push_back(v2);
                    vertices.push_back(v3);
                    vertices.push_back(v1);

                } else
                {
                    KFMTError::error("ModelGLView: Unhandled TMD primitive type (line or sprite).");
                }
            }
            //bind vertex buffer, and copy the data to it
            glFuncs->glBindBuffer(GL_ARRAY_BUFFER, glMesh.frames[i]);
            glFuncs->glBufferData(GL_ARRAY_BUFFER,
                                  vertices.size() * sizeof(MOVertex),
                                  vertices.data(),
                                  GL_STATIC_DRAW);
            //Set vertex number for draw...
            glMesh.numVertex = vertices.size();

            vertices.clear();

            i++;
        }
    }

    //Build MO Shader (only once though)
    if(glMOProgramMVP == 12345678)
    {
        if(!glMOProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/litMime.vert"))
            KFMTError::error("ModelGLView: Couldn't load GLSL Vertex Shader...");

        if(!glMOProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/litCommon.frag"))
            KFMTError::error("ModelGLView: Couldn't load GLSL Fragment Shader...");

        if(!glMOProgram.link())
            KFMTError::error("ModelGLView: Couldn't link GLSL Program...");

        glMOProgramMVP = glMOProgram.uniformLocation("uMVP");
        glMOProgramModel = glMOProgram.uniformLocation("uModel");
        glMOProgramLightPos = glMOProgram.uniformLocation("uLightPos");
        glMOProgramWeight = glMOProgram.uniformLocation("uWeight");
    }

    animFrame = 0;
    animFrameDelta = 0.f;
}

void ModelGLView::DrawMOAnimation()
{
    //Set up the shader...
    glMOProgram.bind();
    glMOProgram.setUniformValue(glMOProgramMVP, (glMatProj * glMatView) * glMatWorld);
    glMOProgram.setUniformValue(glMOProgramModel, glMatWorld);
    glMOProgram.setUniformValue(glMOProgramLightPos, glCamFrom);
    glMOProgram.setUniformValue(glMOProgramWeight, animFrameDelta);

    //Draw it...
    glFuncs->glBindVertexArray(meshes[curAnim].VAO);
    glFuncs->glBindBuffer(GL_ARRAY_BUFFER, meshes[curAnim].frames[animFrame]);
    glFuncs->glDrawArrays(GL_TRIANGLES, 0, meshes[curAnim].numVertex);

    //Set up the offsets and strides of each component of the vertex...

    //Position 1
    glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 60, nullptr);
    glFuncs->glEnableVertexAttribArray(0);

    //Position 2
    glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 60, reinterpret_cast<void*>(12));
    glFuncs->glEnableVertexAttribArray(1);

    //Normal
    glFuncs->glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 60, reinterpret_cast<void*>(24));
    glFuncs->glEnableVertexAttribArray(2);

    //Colour
    glFuncs->glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 60, reinterpret_cast<void*>(36));
    glFuncs->glEnableVertexAttribArray(3);

    //Texcoord
    glFuncs->glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, 60, reinterpret_cast<void*>(52));
    glFuncs->glEnableVertexAttribArray(4);

    glFuncs->glBindVertexArray(0);

    //Some quick 'n' dirty logic to make things move
    animFrameDelta += animFrameAdd;

    if (animFrameDelta >= 1.f)
    {
        animFrame = (animFrame + 1) % meshes[curAnim].frames.size();
        animFrameDelta = 0.f;
    }
}

//
// TMD/RTMD Build & Draw
//
void ModelGLView::BuildTMDModel()
{
    //Clear GLMesh if a new TMD is built
    for (GLMesh& mesh : meshes)
    {
        glFuncs->glDeleteVertexArrays(1, &mesh.VAO);
        glFuncs->glDeleteBuffers(mesh.frames.size(), mesh.frames.data());
    }
    meshes.clear();

    //Build each TMDObject as a GLMesh
    for (const Model::Mesh& tmdObj : model->baseObjects)
    {
        std::vector<TMDVertex> vertices;
        GLMesh& mesh = meshes.emplace_back();
        mesh.frames.resize(1);

        for (const Model::Primitive& tmdPrim : tmdObj.primitives)
        {
            if (tmdPrim.isTriangle())
            {
                //Add 3 vertices to the list
                TMDVertex v0, v1, v2;

                //Vertex 1
                v0.position = tmdObj.vertices[tmdPrim.vertex0];
                v0.normal = tmdObj.normals[tmdPrim.normal0];
                v0.colour = tmdPrim.Colour0();
                v0.texcoord = {tmdPrim.u0 / 255.f, tmdPrim.v0 / 255.f};

                //Vertex 2
                v1.position = tmdObj.vertices[tmdPrim.vertex1];
                v1.normal = tmdPrim.isSmooth() ? tmdObj.normals[tmdPrim.normal1]
                                               : tmdObj.normals[tmdPrim.normal0];
                v1.colour = tmdPrim.isGradation() ? tmdPrim.Colour1() : tmdPrim.Colour0();
                v1.texcoord = {tmdPrim.u1 / 255.f, tmdPrim.v1 / 255.f};

                //Vertex 2
                v2.position = tmdObj.vertices[tmdPrim.vertex2];
                v2.normal = tmdPrim.isSmooth() ? tmdObj.normals[tmdPrim.normal2]
                                               : tmdObj.normals[tmdPrim.normal0];
                v2.colour = tmdPrim.isGradation() ? tmdPrim.Colour2() : tmdPrim.Colour0();
                v2.texcoord = {tmdPrim.u2 / 255.f, tmdPrim.v2 / 255.f};

                vertices.push_back(v2);
                vertices.push_back(v1);
                vertices.push_back(v0);

                if (tmdPrim.isDoubleSided()) //Just reverse-wind the previous triangle
                {
                    vertices.push_back(v0);
                    vertices.push_back(v2);
                    vertices.push_back(v1);
                }
            } else if (tmdPrim.isQuad())
            {
                //Add 4 vertices to the list
                TMDVertex v0, v1, v2, v3;

                //Vertex 1
                v0.position = tmdObj.vertices[tmdPrim.vertex0];
                v0.normal = tmdObj.normals[tmdPrim.normal0];
                v0.colour = tmdPrim.Colour0();
                v0.texcoord = {tmdPrim.u0 / 255.f, tmdPrim.v0 / 255.f};

                //Vertex 2
                v1.position = tmdObj.vertices[tmdPrim.vertex1];
                v1.normal = tmdPrim.isSmooth() ? tmdObj.normals[tmdPrim.normal1]
                                               : tmdObj.normals[tmdPrim.normal0];
                v1.colour = tmdPrim.isGradation() ? tmdPrim.Colour1() : tmdPrim.Colour0();
                v1.texcoord = {tmdPrim.u1 / 255.f, tmdPrim.v1 / 255.f};

                //Vertex 2
                v2.position = tmdObj.vertices[tmdPrim.vertex2];
                v2.normal = tmdPrim.isSmooth() ? tmdObj.normals[tmdPrim.normal2]
                                               : tmdObj.normals[tmdPrim.normal0];
                v2.colour = tmdPrim.isGradation() ? tmdPrim.Colour2() : tmdPrim.Colour0();
                v2.texcoord = {tmdPrim.u2 / 255.f, tmdPrim.v2 / 255.f};

                //Vertex 2
                v3.position = tmdObj.vertices[tmdPrim.vertex3];
                v3.normal = tmdPrim.isSmooth() ? tmdObj.normals[tmdPrim.normal3]
                                               : tmdObj.normals[tmdPrim.normal0];
                v3.colour = tmdPrim.isGradation() ? tmdPrim.Colour3() : tmdPrim.Colour0();
                v3.texcoord = {tmdPrim.u3 / 255.f, tmdPrim.v3 / 255.f};

                //Split the quad into two seperate triangles

                //Tri 1 (0, 1, 2)
                vertices.push_back(v2);
                vertices.push_back(v1);
                vertices.push_back(v0);

                //Tri 2 (2, 3, 1)
                vertices.push_back(v2);
                vertices.push_back(v3);
                vertices.push_back(v1);

                if (tmdPrim.isDoubleSided()) //Just reverse-wind the previous triangle
                {
                    //Tri 1 (0, 1, 2)
                    vertices.push_back(v0);
                    vertices.push_back(v2);
                    vertices.push_back(v1);

                    //Tri 2 (1, 3, 2)
                    vertices.push_back(v1);
                    vertices.push_back(v2);
                    vertices.push_back(v3);
                }
            } else
                KFMTError::error("ModelGLView: Unhandled TMD primitive type (line or sprite).");
        }

        //Begin Generating OpenGL stuff for this object
        glFuncs->glGenBuffers(1, mesh.frames.data());
        glFuncs->glGenVertexArrays(1, &mesh.VAO);

        //Bind array buffer
        glFuncs->glBindVertexArray(mesh.VAO);

        //bind vertex buffer, and copy the data to it
        glFuncs->glBindBuffer(GL_ARRAY_BUFFER, mesh.frames[0]);
        glFuncs->glBufferData(GL_ARRAY_BUFFER,
                              vertices.size() * sizeof(TMDVertex),
                              &vertices.front(),
                              GL_STATIC_DRAW);

        //Set up the offsets and strides of each component of the vertex...

        //Position
        glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 48, nullptr);
        glFuncs->glEnableVertexAttribArray(0);

        //Normal
        glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 48, reinterpret_cast<void*>(12));
        glFuncs->glEnableVertexAttribArray(1);

        //Colour
        glFuncs->glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 48, reinterpret_cast<void*>(24));
        glFuncs->glEnableVertexAttribArray(2);

        //Texcoord
        glFuncs->glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 48, reinterpret_cast<void*>(40));
        glFuncs->glEnableVertexAttribArray(3);

        //Unbind buffers
        glFuncs->glBindVertexArray(0);

        //Set vertex number for draw...
        mesh.numVertex = vertices.size();

        meshes.push_back(mesh);
    }

    if (model->baseObjects.size() > 1)
    {
        for (size_t i = 1; i < meshes.size(); i++) model->baseObjects[i].visible = false;
    }

    //Now we build the shaders required for rendering
    if (!glTMDProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/litStatic.vert"))
        KFMTError::error("ModelGLView: Couldn't load GLSL Vertex Shader...");

    if (!glTMDProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/litCommon.frag"))
        KFMTError::error("ModelGLView: Couldn't load GLSL Fragment Shader...");

    if (!glTMDProgram.link()) KFMTError::error("ModelGLView: Couldn't link GLSL Program...");

    glTMDProgramMVP = glTMDProgram.uniformLocation("uMVP");
    glTMDProgramModel = glTMDProgram.uniformLocation("uModel");
    glTMDProgramLightPos = glTMDProgram.uniformLocation("uLightPos");
}

void ModelGLView::DrawTMDModel()
{
    //Bind GLSL Program
    glTMDProgram.bind();
    glTMDProgram.setUniformValue(glTMDProgramMVP, (glMatProj * glMatView) * glMatWorld);
    glTMDProgram.setUniformValue(glTMDProgramModel, glMatWorld);
    glTMDProgram.setUniformValue(glTMDProgramLightPos, glCamFrom);

    //Render each object
    for (size_t i = 0; i < model->baseObjects.size(); ++i)
    {
        //Don't render this object if it is not visible.
        if (!model->baseObjects[i].visible) continue;

        glFuncs->glBindVertexArray(meshes[i].VAO);
        glFuncs->glDrawArrays(GL_TRIANGLES, 0, meshes[i].numVertex);
        glFuncs->glBindVertexArray(0);
    }
}

//
// GL Grid
//
void ModelGLView::BuildGrid()
{
    gridMesh.frames.resize(1);
    glFuncs->glGenBuffers(1, gridMesh.frames.data());
    glFuncs->glGenVertexArrays(1, &gridMesh.VAO);

    glFuncs->glBindVertexArray(gridMesh.VAO);
    glFuncs->glBindBuffer(GL_ARRAY_BUFFER, gridMesh.frames[0]);
    glFuncs->glBufferData(GL_ARRAY_BUFFER, sizeof(gridVertices), gridVertices.data(), GL_STATIC_DRAW);

    glFuncs->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<void *>(0));
    glFuncs->glEnableVertexAttribArray(0);
    glFuncs->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<void *>(12));
    glFuncs->glEnableVertexAttribArray(1);

    glFuncs->glBindBuffer(GL_ARRAY_BUFFER, 0);
    glFuncs->glBindVertexArray(0);

    //Build Shader
    if(!glSimpleProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/unlitSimple.vert"))
        KFMTError::error("ModelGLView: Couldn't load GLSL Vertex Shader...");

    if(!glSimpleProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/unlitSimple.frag"))
        KFMTError::error("ModelGLView: Couldn't load GLSL Fragment Shader...");

    if(!glSimpleProgram.link())
        KFMTError::error("ModelGLView: Couldn't link GLSL Program...");

    glSimpleProgramMVP = glSimpleProgram.uniformLocation("uMVP");
}

void ModelGLView::DrawGrid()
{
    //Bind GLSL Program
    glSimpleProgram.bind();
    glSimpleProgram.setUniformValue(glSimpleProgramMVP, (glMatProj * glMatView));

    glFuncs->glBindVertexArray(gridMesh.VAO);
    glFuncs->glDrawArrays(GL_LINES, 0, 46);

    glFuncs->glBindVertexArray(0);
}