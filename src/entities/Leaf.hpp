class Leaf{
    //MOVE TO LEAF CLASS
    const int leafPoints = 5;
    const float jagednessFactor_Leaf = 0.35;
    const float textureLeafStart = 0.9f;
    const float textureLeafEnd = 1.0f;
    std::vector<glm::vec3>* leafVertices = combinedVertices; 	std::vector<GLuint>* leafIndices= combinedIndices;		std::vector<glm::vec3>* leafColor= combinedColor; std::vector<std::vector<int>>* leafStartIndices= combinedStartIndices;
    std::vector<glm::vec2>* leafUVs = combinedUV;
    glm::vec3 green = glm::vec3(30.0 / 255, 147.0 / 255, 45.0 / 255);
};