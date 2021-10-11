#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <ostream>
#include <string>

#include <Camera.h>
#include <Model.h>

#include <glm/matrix.hpp>
#include <map>
#include <Utils.h>

using namespace std;

bool firstMove = true;
float lastX = 0.0f;
float lastY = 0.0f;
float lastTime = (float)glfwGetTime();
int WIDTH = 1920;
int HEIGHT = 1080;
float cosThetaThreshold = 0.372;
float removeEdgeThreshold = 0.95;
unsigned int removeEdgeCount = 0;
float buttonDelay = 0;
// bool allowCursorPosCallback = false;

Camera mainCamera = Camera(glm::vec3(0.0f, 9.0f, 7.0f));

void frame_buffer_size_callback(GLFWwindow *, int width, int height);
void window_pos_callback(GLFWwindow * window, double xPos, double yPos);
void scroll_callback(GLFWwindow *, double, double);

void processInput(GLFWwindow * );


static bool bCursorOff = false;
static bool bPressed;

static void switch_cursor(GLFWwindow * window)
{
    if (!bCursorOff)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, window_pos_callback);
    }
    else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        glfwSetCursorPosCallback(window, nullptr);
        firstMove = true;
    }
    bCursorOff = !bCursorOff;
}

Shader * shader = nullptr;
Shader * postprocessShader = nullptr;
Shader * gBufferShader = nullptr;
Shader * debugQuad = nullptr;
Shader * pointShader = nullptr;
unsigned int framebuffer, frameTexture, renderbuffer, ppFramebuffer, ppFrameTexture;
float scaleFactor = 1.0f;
unsigned int pointsVAO, pointsVBO;

void recompileShaders()
{
    if (!shader)
    {
        shader = new Shader("Others/ModelVS33.vert", "Others/ModelFS33.frag", nullptr);
    }
    else
    {
        shader->recompileFromSource();
    }
    
    if (!postprocessShader)
    {
        postprocessShader = new Shader("Others/PostProcessingSharpen.vert", "Others/PostProcessingSharpen.frag", nullptr);
    }
    else
    {
        postprocessShader->recompileFromSource();
    }
    if (!gBufferShader)
    {
        gBufferShader = new Shader("Others/GBufferVS.vert", "Others/GBufferFS.frag", nullptr);
    }
    else
    {
        gBufferShader->recompileFromSource();
    }
    if (!debugQuad)
    {
        debugQuad = new Shader("Others/DebugQuadVS.vert", "Others/DebugQuadFS.frag", nullptr);
    }
    else
    {
        debugQuad->recompileFromSource();
    }
    if (!pointShader)
    {
        pointShader = new Shader("Others/Point.vert", "Others/Point.frag", "Others/Point.geom");
    }
    else
    {
        pointShader->recompileFromSource();
    }
    
    
}

unsigned int gBufferFBO, gPosition, gNormal, gAlbedoSpec, depthRB;

void onWindowSizeChanged()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    glDeleteTextures(1, &frameTexture);
    glGenTextures(1, &frameTexture);
    glBindTexture(GL_TEXTURE_2D, frameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameTexture, 0);


    glDeleteRenderbuffers(1, &renderbuffer);
    glGenRenderbuffers(1, &renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "framebuffer incomplete!" << endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, ppFramebuffer);

    glDeleteTextures(1, &ppFrameTexture);
    glGenTextures(1, &ppFrameTexture);
    glBindTexture(GL_TEXTURE_2D, ppFrameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ppFrameTexture, 0);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "framebuffer incomplete!" << endl;
    }



    glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
    

    // position color buffer
    glDeleteTextures(1,  &gPosition);
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // normal color buffer
    glDeleteTextures(1, &gNormal);
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // color + specular color buffer
    glDeleteTextures(1, &gAlbedoSpec);
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 }; 
    glDrawBuffers(3, attachments);
    
    glDeleteRenderbuffers(1, &depthRB);
    glGenRenderbuffers(1, &depthRB);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRB);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        cout << "ERROR::FRAME BUFFER INIT FAILED!" <<endl;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

struct VertexMerged
{
    glm::vec3 Position;
    std::vector<glm::vec3> Normals;
    std::vector<glm::vec2> TexCoords;
    std::vector<glm::vec3> Tangents;
    VertexMerged(Vertex vertex){
        Position = vertex.Position;
        Normals = std::vector<glm::vec3>{vertex.Normal};
        TexCoords = std::vector<glm::vec2>{vertex.TexCoords};
        Tangents = std::vector<glm::vec3>{vertex.Tangent};
    }
    VertexMerged(){}
};

bool SamePos(glm::vec3& v1, float x, float y , float z)
{
    return v1.x == x && v1.y == y && v1.z == z;
}
bool SamePos(glm::vec3 &v1, glm::vec3 &v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

bool SameVertex(Vertex & v1, Vertex &v2)
{
    return SamePos(v1.Position, v2.Position);
}

ostream& operator << (ostream &os, glm::vec3 v3)
{
    return os << "(" << v3.x << "," << v3.y << "," << v3.z << ")";
}

glm::vec3 Normalize(glm::vec3 v3)
{
    float sqrtRes = sqrt(v3.x * v3.x + v3.y * v3.y + v3.z * v3.z);
    return glm::vec3(v3.x / sqrtRes, v3.y / sqrtRes, v3.z / sqrtRes);
}


bool ShouldPickThisPoint(std::vector<glm::vec3> &Normals)
{
    bool result = false;
    for (glm::vec3 &TargetNormal : Normals)
    {
        glm::vec3 NTargetNormal = Normalize(TargetNormal);
        for (glm::vec3 & Normal : Normals)
        {
            glm::vec3 NNormal = Normalize(Normal);
            if (glm::dot(NTargetNormal, NNormal) < cosThetaThreshold)
            {
                result = true;
                break;
            }
        }
        if (result)
        {
            break;
        }
    }
    return result;
}
struct Triangle
{
    int id;
    Vertex vert0;
    Vertex vert1;
    Vertex vert2;
};

std::vector<glm::vec3> diffVertexPosition;
std::map<int, std::vector<Triangle>> vertex2Triangle;
std::vector<float> allPoints = std::vector<float>();

void CheckAndAddTriangle(Vertex&triVert, Vertex targetVert, Triangle &tri, int key)
{
    if (triVert.Position.x == targetVert.Position.x &&
        triVert.Position.y == targetVert.Position.y &&
        triVert.Position.z == targetVert.Position.z)
    {
        auto & result = vertex2Triangle.find(key);
        if (result != vertex2Triangle.end())
        {
            auto & entry = vertex2Triangle[key];
            entry.push_back(tri);
        }
        else
        {
            vertex2Triangle[key] = std::vector<Triangle>({tri});
        }
    }
}
void InitVertex2Triangle(Model & model)
{
    vertex2Triangle.clear();
    std::vector<Triangle> allTriangles;
    std::vector<Mesh>& meshs = model.GetMeshes();
    for(Mesh &mesh : meshs)
    {
        int triangleId = 1;
        for (unsigned int index = 0; index < (mesh.indices.size() / 3); ++index)
        {
            Triangle triangle;
            triangle.vert0 = mesh.vertices[mesh.indices[index * 3]];
            triangle.vert1 = mesh.vertices[mesh.indices[index * 3 + 1]];
            triangle.vert2 = mesh.vertices[mesh.indices[index * 3 + 2]];
            triangle.id = triangleId ++ ;
            allTriangles.push_back(triangle);
        }
        unsigned int vertInVertices = 0;
        for (Vertex & vert : mesh.vertices)
        {
            unsigned int key = -1;
            for (unsigned int f = 0; f < diffVertexPosition.size(); ++f)
            {
                if (SamePos(vert.Position, diffVertexPosition[f]))
                {
                    key = f;
                    break;
                }
            }
            if (key == -1)
            {
                key = diffVertexPosition.size();
                diffVertexPosition.push_back(vert.Position);
            }
            for(Triangle & tri : allTriangles)
            {
                // 分别检查3个点，是否为给定点，如果是则添加三角形到数组中
                CheckAndAddTriangle(tri.vert0, vert, tri, key);
                CheckAndAddTriangle(tri.vert1, vert, tri, key);
                CheckAndAddTriangle(tri.vert2, vert, tri, key);
            }
            vertInVertices ++;
        }
    }
}
std::vector<Triangle> & FindTriangles(Vertex & inVertex, std::vector<Vertex> & vertices)
{
    unsigned int vertInVertices = 0;
    for (Vertex & vertex : vertices)
    {
        if (SamePos(vertex.Position, inVertex.Position) )
        {
            unsigned int key = -1;
            for (unsigned int k = 0; k < diffVertexPosition.size(); ++k)
            {
                if (SamePos(vertex.Position, diffVertexPosition[k]))
                {
                    key = k;
                    break;
                }
            }
            // should not be -1
            return vertex2Triangle[key];
        }
        vertInVertices ++;
    }
}

glm::vec3 CalNormalByCross(Triangle & tri, Vertex & v)
{
    if (SameVertex(tri.vert0, v))
    {
        return glm::normalize(glm::cross(tri.vert2.Position - tri.vert0.Position, tri.vert1.Position - tri.vert0.Position));
    }
    if (SameVertex(tri.vert1, v))
    {
        return glm::normalize(glm::cross(tri.vert0.Position - tri.vert1.Position, tri.vert2.Position - tri.vert1.Position));
    }
    if (SameVertex(tri.vert2, v))
    {
        return glm::normalize(glm::cross(tri.vert1.Position - tri.vert2.Position, tri.vert0.Position - tri.vert2.Position));
    }
}

Vertex & FindSameVertexInTriangle(Triangle tri, Vertex v)
{
    if (SameVertex(tri.vert0, v))
    {
        return tri.vert0;
    }
    if (SameVertex(tri.vert1, v))
    {
        return tri.vert1;
    }
    if (SameVertex(tri.vert2, v))
    {
        return tri.vert2;
    }
}

bool ShouldRemoveEdge(std::vector<Triangle> & edgeSharedTriangles, Vertex & v0, Vertex & v1)
{
    bool removedEdge = true;
    for (unsigned int i = 0; i < edgeSharedTriangles.size() ; ++i)
    {
        for (unsigned int j = i + 1; j < edgeSharedTriangles.size(); ++j)
        {
            Triangle triBase = edgeSharedTriangles[i];
            Triangle triTarget = edgeSharedTriangles[j];
            float dotProduct = glm::dot(CalNormalByCross(triBase, v0), CalNormalByCross(triTarget, v0));
            if (dotProduct < removeEdgeThreshold && dotProduct > -removeEdgeThreshold)
            {
                removedEdge = false;
                break;
            }
            dotProduct = glm::dot(CalNormalByCross(triBase, v1), CalNormalByCross(triTarget, v1));
            if (dotProduct < removeEdgeThreshold && dotProduct > -removeEdgeThreshold)
            {
                removedEdge = false;
                break;
            }
        }
    }
    return removedEdge;
}
std::vector<float> removedEdgeData0, removedEdgeData1, removedEdgeData;
void MarkVertexAsEdge(Model& model)
{
    allPoints.clear();
    removedEdgeData0.clear();
    removedEdgeData1.clear();
    removedEdgeData.clear();
    removeEdgeCount = 0;
    std::vector<Mesh>& meshs = model.GetMeshes();
    std::vector<VertexMerged> vertexVector;
    for(Mesh &mesh : meshs)
    {
        vertexVector.clear();
        for(Vertex & vertex : mesh.vertices)
        {
            glm::vec3 pos = vertex.Position;
            bool newOne = true;
            for (VertexMerged &mergedData : vertexVector)
            {
                if (SamePos(mergedData.Position, pos))
                {
                    
                    mergedData.Normals.push_back(vertex.Normal);
                    mergedData.TexCoords.push_back(vertex.TexCoords);
                    mergedData.Tangents.push_back(vertex.Tangent);
                    newOne = false;
                    break;
                }
            }
            if (newOne)
            {
                vertexVector.push_back(VertexMerged(vertex));
            }
        }
        std::vector<VertexMerged> picked = std::vector<VertexMerged>();
        for (auto it = vertexVector.begin();it != vertexVector.end(); ++it)
        {
            VertexMerged merged = *it;
            glm::vec3 Position = merged.Position;
            if (merged.Normals.size() > 0)
            {
                if (ShouldPickThisPoint(merged.Normals))
                {
                    picked.push_back(merged);
                }
            }
        }
        cout << " picked size : " << picked.size() << ", mesh.indices : " << mesh.indices.size() << endl;
        std::vector<Vertex> triangle_all_picked;
        std::vector<glm::vec4> cachedData;
        unsigned int count = 0;
        for (int index : mesh.indices)
        {
            count ++;
            std::vector<Vertex> vertices = mesh.vertices;
            glm::vec3 pos = vertices[index].Position;
            glm::vec4 cachedDataElem(pos.x, pos.y, pos.z, 0.0f);
            
            for (VertexMerged &mergedData : picked)
            {
                if (mergedData.Position.x == pos.x && 
                mergedData.Position.y == pos.y &&
                mergedData.Position.z == pos.z)
                {
                    cachedDataElem.w = mergedData.Normals.size();
                    triangle_all_picked.push_back(vertices[index]);
                    break;
                }
            }
            cachedData.push_back(cachedDataElem);
            // ** 对于所有pick了3个顶点的三角形，检查每一条边，与该边共边的其他三角形的法线与该三角形的法线dotproduct是否为0，为0则标记数据（在所有数据中都不渲染这条边）
            if (count == 3)
            {
                std::vector<unsigned int> removeEdgeInfo;
                if (triangle_all_picked.size() > 1)
                {
                    unsigned int edgeCount = 3;
                    if (triangle_all_picked.size() == 2)
                    {
                        edgeCount = 1;
                    }
                    for (unsigned int edge = 0; edge < edgeCount; ++edge)
                    {
                        Vertex & vertex0 = triangle_all_picked[edge];
                        Vertex & vertex1 = triangle_all_picked[(edge + 1) % 3];
                        // ** 找到两个顶点对应的所有三角形，求交集得到所有共边的三角形
                        std::vector<Triangle> vertex0Triangles = FindTriangles(vertex0, vertices);
                        std::vector<Triangle> vertex1Triangles = FindTriangles(vertex1, vertices);
                        std::vector<Triangle> edgeSharedTriangles;
                        for (Triangle & tri : vertex0Triangles)
                        {
                            for (Triangle & tri1 : vertex1Triangles)
                            {
                                if (tri1.id == tri.id)
                                {
                                    edgeSharedTriangles.push_back(tri);
                                }
                            }
                        }
                        bool removeEdge = ShouldRemoveEdge(edgeSharedTriangles, vertex0, vertex1);
                        if (removeEdge)
                        {
                            removeEdgeInfo.push_back(edge);
                            removeEdgeCount++;
                        }
                    }
                }

                // arrange data
                for (unsigned int i = 0; i < cachedData.size() ; ++i)
                {
                    glm::vec4 data = cachedData[i];
                    allPoints.push_back(data.x);
                    allPoints.push_back(data.y);
                    allPoints.push_back(data.z);
                    allPoints.push_back(data.w);
                    unsigned int a = 0, b = 0;
                    if (removeEdgeInfo.size() > 0)
                    {
                        if (triangle_all_picked.size() == 3)
                        {
                            for (auto edgeInfo : removeEdgeInfo)
                            {
                                if (i == 0)
                                {
                                    a = edgeInfo == 0 ? 1 : a;
                                    b = edgeInfo == 2 ? 1 : b;
                                }
                                if (i == 1)
                                {
                                    a = edgeInfo == 1 ? 1 : a;
                                    b = edgeInfo == 0 ? 1 : b;
                                }
                                if (i == 2)
                                {
                                    a = edgeInfo == 2 ? 1 : a;
                                    b = edgeInfo == 1 ? 1 : b;
                                }

                            }
                        }
                        else
                        {
                            // 仅有的一条线需要删除
                            Vertex& v0 = triangle_all_picked[0];
                            Vertex& v1 = triangle_all_picked[1];
                            Vertex Target;
                            if (SamePos(v0.Position, data.x, data.y, data.z))
                            {
                                Target = v1;
                            }
                            if (SamePos(v1.Position, data.x, data.y, data.z))
                            {
                                Target = v0;
                            }
                            if (i == 0)
                            {
                                if (SamePos(Target.Position, cachedData[1].x, cachedData[1].y, cachedData[1].z))
                                {
                                    a = 1;
                                }
                                if (SamePos(Target.Position, cachedData[2].x, cachedData[2].y, cachedData[2].z))
                                {
                                    b = 1;
                                }
                            }
                            if (i == 1)
                            {
                                if (SamePos(Target.Position, cachedData[0].x, cachedData[0].y, cachedData[0].z))
                                {
                                    b = 1;
                                }
                                if (SamePos(Target.Position, cachedData[2].x, cachedData[2].y, cachedData[2].z))
                                {
                                    a = 1;
                                }
                            }
                            if (i == 2)
                            {
                                if (SamePos(Target.Position, cachedData[0].x, cachedData[0].y, cachedData[0].z))
                                {
                                    a = 1;
                                }
                                if (SamePos(Target.Position, cachedData[1].x, cachedData[1].y, cachedData[1].z))
                                {
                                    b = 1;
                                }
                            }
                        }
                    }
                    allPoints.push_back(a);
                    allPoints.push_back(b);
                }
                triangle_all_picked.clear();
                count = 0;
                cachedData.clear();
            }
            // cout <<" index : " << index << " pos : "<< pos  <<"normals : " << *(--allPoints.end()) << endl;
        }
        cout << "removeEdgeCount : " << removeEdgeCount <<endl;
    }
}

void RebindBufferdata()
{
    glBindVertexArray(pointsVAO);

    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * allPoints.size(), &allPoints[0]);
    // glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * allPoints.size(), sizeof(float) * removedEdgeData.size(), &removedEdgeData[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow * window = glfwCreateWindow(WIDTH, HEIGHT, "Model", NULL, NULL);
    if (window == NULL)
    {
        cout << "ERROR::CREATE WINDOW FAILED!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "ERROR::GLAD LOAD FAILED!" << endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
    glfwGetFramebufferSize(window, &WIDTH, &HEIGHT);
    glfwSetScrollCallback(window, scroll_callback);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    Model backpack = Model("Others/Model/jdxc.obj", true);
    Model polySurface = Model("Others/Model/polySurface.obj", true);
    InitVertex2Triangle(polySurface);
    recompileShaders();
    MarkVertexAsEdge(polySurface);
    

    float materialShininess = 32.0f;
    glm::vec3 pointLightPosition = glm::vec3(5.0, 5.0f, 5.0f);
    glm::vec3 clear_color = glm::vec3(0);
    float pointAmbientStrength = 0.2f;
    float pointDiffuseStrength = 0.6f;
    float pointSpecularStrength = 1.0f;

    float constant = 1.0f;
    float linear = 0.2f;
    float quadratic = 0.07f;

    glGenFramebuffers(1, &framebuffer);
    glGenFramebuffers(1, &gBufferFBO);
    glGenFramebuffers(1, &ppFramebuffer);

    unsigned int planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    float vertices[30] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    glGenVertexArrays(1, &pointsVAO);
    glBindVertexArray(pointsVAO);
    glGenBuffers(1, &pointsVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pointsVBO);
    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * allPoints.size(), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * allPoints.size(), &allPoints[0]);
    // glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * allPoints.size(), sizeof(float) * removedEdgeData.size(), &removedEdgeData[0]);
    // glBufferSubData(GL_ARRAY_BUFFER, sizeof(float) * allPoints.size(), sizeof(float) * removedEdgeData.size(), &removedEdgeData[0]);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float ) * 6, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(float ) * 6, (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float ) * 6, (void *)(sizeof(float) * 4));
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float ) * 6, (void *)(sizeof(float) * 5));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    RebindBufferdata();

    glBindVertexArray(0);
    onWindowSizeChanged();


    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Editor");
        {
            ImGui::InputFloat("Material shininess", &materialShininess);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            ImGui::Spacing();
            ImGui::Text("Lights Properties");
            ImGui::Spacing();
            ImGui::SliderFloat("pointAmbientStrength", &pointAmbientStrength, 0, 1);
            ImGui::SliderFloat("pointDiffuseStrength", &pointDiffuseStrength, 0, 1);
            ImGui::SliderFloat("pointSpecularStrength", &pointSpecularStrength, 0, 1);

            ImGui::Spacing();
            ImGui::SliderFloat("constant", &constant, 0, 1);
            ImGui::SliderFloat("linear", &linear, 0, 1);
            ImGui::SliderFloat("quadratic", &quadratic, 0, 1);

            ImGui::Separator();
            if (bCursorOff)
            {
                ImGui::Text("Press P to release control of the camera, and show cursor.");
            }
            else
            {
                ImGui::Text("Press P or Button follows to take control of the camera");
                if(ImGui::Button("Posses camera") && !bCursorOff)
                {
                    switch_cursor(window);
                }
            }
            if (ImGui::Button("Recompile Shaders"))
            {
                recompileShaders();
            }
            ImGui::SliderFloat("Scale Factor", &scaleFactor, 0.1, 100.0f);
            ImGui::Separator();
            ImGui::SliderFloat("Camera Speed", (float *)&mainCamera.MovementSpeed, .0f, 5.0f);
            ImGui::SliderFloat("Camera Sensitivity", (float *)&mainCamera.MouseSensitivity, .0f, .3f);

            ImGui::SliderFloat("cosTheta Threshold", &cosThetaThreshold, -1, 1);
            ImGui::SliderFloat("removeEdge Threshold", &removeEdgeThreshold, 0, 1);
            if (ImGui::Button("Recalculate Picked Points") && buttonDelay <= 0)
            {
                buttonDelay = 5.0f;
                MarkVertexAsEdge(polySurface);
                RebindBufferdata();
            }
            glm::vec3 pos = mainCamera.Position;
            ImGui::Text("Camera Position (%.1f, %.1f, %.1f)", pos.x, pos.y, pos.z);
            ImGui::Text("Camera Yaw (%.1f), Pitch (%.1f)", mainCamera.Yaw, mainCamera.Pitch);
            ImGui::Separator();

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

            ImGui::End();
        }

        // Rendering
        ImGui::Render();

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(scaleFactor));
        
        glm::mat4 view = mainCamera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(mainCamera.Zoom), (float)WIDTH/HEIGHT, 0.1f, 100.0f);

        shader->use();

        shader->setMat4("model", model);
        shader->setMat4("view", view);
        shader->setMat4("projection", projection);

        shader->setVec3("viewPos", mainCamera.Position);
        shader->setFloat("material.shininess", materialShininess);

        shader->setVec3("pointLight.position", pointLightPosition);
        shader->setVec3("pointLight.ambient", glm::vec3(pointAmbientStrength));
        shader->setVec3("pointLight.diffuse", glm::vec3(pointDiffuseStrength));
        shader->setVec3("pointLight.specular", glm::vec3(pointSpecularStrength));

        shader->setFloat("pointLight.constant", constant);
        shader->setFloat("pointLight.linear", linear);
        shader->setFloat("pointLight.quadratic", quadratic);

        polySurface.Draw(*shader);

        glBindFramebuffer(GL_FRAMEBUFFER, ppFramebuffer);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameTexture);
        postprocessShader->use();
        // postprocessShader->setMat4("model", glm::mat4(1.0f));
        glBindVertexArray(planeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        glBindFramebuffer(GL_FRAMEBUFFER, gBufferFBO);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gBufferShader->use();
        gBufferShader->setMat4("model", model);
        gBufferShader->setMat4("view", view);
        gBufferShader->setMat4("projection", projection);
        
        polySurface.Draw(*gBufferShader);



        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT);
        debugQuad->use();
        glm::mat4 result = glm::matrixCompMult(projection, view);
        debugQuad->setMat4("WorldToNDC", result);
        debugQuad->setMat4("view", view);
        debugQuad->setMat4("projection", projection);
        debugQuad->setBool("renderMix", false);
        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5));
        glm::mat4 bottomLeft = glm::translate(model, glm::vec3(-1, -1, 0.0f));
        debugQuad->setMat4("model", bottomLeft);
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glm::mat4 topRight = glm::translate(model, glm::vec3(1, 1, 0.0f));
        debugQuad->setMat4("model", topRight);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glm::mat4 topLeft = glm::translate(model, glm::vec3(-1, 1, 0.0f));
        debugQuad->setMat4("model", topLeft);
        glBindTexture(GL_TEXTURE_2D, ppFrameTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // draw marked points
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);

        glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pointShader->use();
        model = glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));
        pointShader->setMat4("model", model);
        pointShader->setMat4("view", view);
        pointShader->setMat4("projection", projection);
        glBindVertexArray(pointsVAO);
        int pointCount = allPoints.size() / 6;
        glDrawArrays(GL_TRIANGLES, 0, pointCount);
        

        // draw last texture in main frame
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        debugQuad->use();
        model = glm::scale(glm::mat4(1.0f), glm::vec3(0.5));
        glm::mat4 bottomRight = glm::translate(model, glm::vec3(1, -1, 0.0f));
        debugQuad->setMat4("WorldToNDC", result);
        debugQuad->setMat4("view", view);
        debugQuad->setMat4("projection", projection);
        debugQuad->setMat4("model", bottomRight);
        debugQuad->setBool("renderMix", true);
        debugQuad->setInt("renderTexture", 0);
        debugQuad->setInt("addTexture", 1);
        glBindVertexArray(planeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, frameTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        glfwSwapBuffers(window);
    }
    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void frame_buffer_size_callback(GLFWwindow * window, int width, int height)
{
    if (width > 0 && height > 0)
    {
        WIDTH = width;
        HEIGHT = height;
        onWindowSizeChanged();
        glViewport(0, 0, width, height);
    }
}

void window_pos_callback(GLFWwindow * window, double xPos, double yPos)
{
    if ( firstMove )
    {
        lastX = (float)xPos;
        lastY = (float)yPos;
        firstMove = false;
    }

    float offsetX = (float)xPos - lastX;
    float offsetY = lastY - (float)yPos;
    lastX = (float)xPos;
    lastY = (float)yPos;

    mainCamera.ProcessMouseMovement(offsetX, offsetY);
}

void scroll_callback(GLFWwindow * window, double offsetX, double offsetY)
{
    mainCamera.ProcessMouseScroll((float)offsetY);
}

void processInput(GLFWwindow * window)
{
    float currentTime = (float)glfwGetTime();
    float deltaTime = currentTime - lastTime;
    lastTime = currentTime;

    if (buttonDelay >= 0)
    {
        buttonDelay -= deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        bPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE && bPressed)
    {
        bPressed = false;
        switch_cursor(window);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        mainCamera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}