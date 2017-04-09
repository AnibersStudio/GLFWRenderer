#include "RenderController.h"
#include "GLConstManager.h"
#include "RenderStageImp.h"
RenderController::RenderController(MeshManager & mm, unsigned int w, unsigned int h) : meshmanager(mm), width(w), height(h)
{
	depthstage.Init();
}

void RenderController::Draw(RenderContext context)
{
	RenderPrepare(context);
	///Calculate WVP
	glm::mat4 V = glm::lookAt(context.eye, context.target, context.up);
	glm::mat4 P = glm::perspective(context.FieldOfView / width * height, width / (float)height, 0.2f, context.ViewDistance);
	glm::mat4 WVP = P * V;
	///Calculate Opace Vertices Count
	unsigned int OpaceVerticesCount = 0;
	for (auto & v : framedata.OpaceCount)
	{
		OpaceVerticesCount += v;
	}
	///Calculate Trans Vertices Count
	unsigned int TransVerticesCount = 0;
	for (auto & v : framedata.FullTransCount)
	{
		TransVerticesCount += v;
	}
	for (auto & v : framedata.SemiTransCount)
	{
		TransVerticesCount += v;
	}
	
	depthstage.Prepare(WVP);
	
	static DebugOutput screendrawer{width, height};
	screendrawer.Draw(framedata.Material[0].diffusetex->GetObjectID());

	oldcontext = context;
}

void RenderController::RenderPrepare(RenderContext context)
{
	framedata.Clear();
	meshmanager.AppendMesh(MeshManager::Opace, framedata.Material, framedata.OpaceCount, framedata.Vertex, framedata.MaterialIndex);
	meshmanager.AppendOrderedMesh(MeshManager::FullTrans, context.eye, framedata.Material, framedata.FullTransCount, framedata.Vertex, framedata.MaterialIndex);
	meshmanager.AppendOrderedMesh(MeshManager::SemiTrans, context.eye, framedata.Material, framedata.SemiTransCount, framedata.Vertex, framedata.MaterialIndex);
	for (auto & m : framedata.Material)
	{
		m.diffusetex->SetAF(GLConstManager::GetInstance().GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)[0]);
	}
}
