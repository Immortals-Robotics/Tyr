#version 330 core
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;

// Input uniform values
uniform mat4 mvp;

// Output vertex attributes (to fragment shader)
out vec2 texCoord;
out vec4 fragColor;

void main() {
    // Send vertex attributes to fragment shader
    texCoord = vertexTexCoord;
    // texCoord.y = 1.0 - texCoord.y;
    fragColor = vertexColor;

    // Calculate final vertex position
    gl_Position = mvp*vec4(vertexPosition, 1.0);
}