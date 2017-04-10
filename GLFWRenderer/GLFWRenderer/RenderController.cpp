#include "RenderController.h"
#include "GLConstManager.h"
#include "RenderStageImp.h"
RenderController::RenderController(MeshManager & mm, unsigned int w, unsigned int h) : meshmanager(mm), width(w), height(h), forwardstage(ForwardStage{w, h})
{
	depthstage.Init();
	forwardstage.Init();
}

void RenderController::Draw(RenderContext context)
{
	RenderPrepare(context);
	///Calculate WVPs
	glm::mat4 V = glm::lookAt(context.eye, context.target, context.up);
	V = glm::lookAt(vec3(0.0f), vec3(0.0f, 0.0f, -1.0f), vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 P = glm::perspective(context.FieldOfView / width * height, (float)width / (float)height, 0.2f, context.ViewDistance);
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
	forwardstage.Prepare(framedata, WVP);
	
	//depthstage.Draw(forwardstage.GetVao(), forwardstage.GetFbo(), OpaceVerticesCount);
	forwardstage.Draw(framedata.Material[0].diffusetex->GetObjectID(), OpaceVerticesCount);

	//static DebugOutput screendrawer{width, height};
	//screendrawer.Draw(forwardstage.GetFbo().GetDepthID());

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
