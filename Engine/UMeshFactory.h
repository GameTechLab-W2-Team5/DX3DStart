#pragma once
#include "UMesh.h"
#include "URenderer.h"
#include <vector>

class UMeshFactory
{
public:
    // Sphere Mesh ����
    static UMesh* CreateSphereMesh(URenderer& renderer)
    {
        // ���� �����͸� �� �������� ��ȯ
        int numVertices = sizeof(sphere_vertices) / sizeof(FVertexSimpleOld);
        auto convertedVertices = FVertexSimple::ConvertVertexData(
            sphere_vertices,
            numVertices
        );

        UMesh* mesh = new UMesh();
        mesh->Initialize(renderer.GetDevice(), convertedVertices); // Renderer�� ���� Device ����
        return mesh;
    }
};