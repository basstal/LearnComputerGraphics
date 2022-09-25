#include <Mesh.h>

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    setupMesh();
}

unsigned int Mesh::GetVAO()
{
    return VAO;
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    /*
    if we were to represent a struct as an array of data, it would only contain the struct's variables in sequential order which directly translates to a float (actually byte) array that we want for an array buffer.
    */
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    /* 
    Another great use of structs is a preprocessor directive called offsetof(s,m) that takes as its first argument a struct and as its second argument a variable name of the struct. The macro returns the byte offset of that variable from the start of the struct. 
    */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::BindTexture(const Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int reflectionNr = 1;
    unsigned int tangentNr = 1;

    for (unsigned int i = 0 ; i < textures.size(); ++ i)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].Type;
        if( name == "texture_diffuse" )
            number = std::to_string(diffuseNr ++) ;
        else if( name == "texture_specular" )
            number = std::to_string(specularNr ++) ; 
        else if (name == "texture_reflection")
            number = std::to_string(reflectionNr ++ );
        else if (name == "texture_tangent")
            number = std::to_string(tangentNr ++ );

        shader.setInt(("material." + name + number), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);
    }
}
void Mesh::Draw(const Shader& shader)
{
    BindTexture(shader);
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}