#include "RenderController.h"
#include "GLConstManager.h"
#include "RenderStageImp.h"
RenderController::RenderController(MeshManager & mm, unsigned int w, unsigned int h) : meshmanager(mm), width(w), height(h), forwardstage(ForwardStage{ w, h }), lightcullingstage(LightCullingStage{w, h})
{
	depthstage.Init();

	lightcullingstage.Init(forwardstage.GetFbo());

	forwardstage.Init();
}

void RenderController::Draw(RenderContext context)
{
	RenderPrepare(context);
	///Calculate WVPs
	glm::mat4 V = glm::lookAt(context.eye, context.target, context.up);
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
	lightcullingstage.Prepare(WVP);
	forwardstage.Prepare(framedata, WVP);

	depthstage.Draw(glstate, forwardstage.GetVao(), forwardstage.GetFbo(), OpaceVerticesCount);
	lightcullingstage.Draw(glstate, forwardstage.GetVao(), OpaceVerticesCount, TransVerticesCount);
	//forwardstage.Draw(glstate, OpaceVerticesCount + TransVerticesCount);

	static DebugOutput screendrawer{width, height};
	screendrawer.Draw(lightcullingstage.GetMaxDepth().GetDepthID());
	//screendrawer.Draw(forwardstage.GetFbo().GetDepthID());

	oldcontext = context;


	glm::vec3 a[3]{ glm::vec3{ 0.52573f, 0.00000f, 0.85065f },   glm::vec3{ 0.00000f, 0.85065f, 0.52573f },   glm::vec3{ -0.52573f, 0.00000f, 0.85065f }, };
	glm::vec3 b = a[0] - a[1];
	glm::vec3 c = a[0] - a[2];
	glm::vec3 nor = glm::normalize(glm::cross(c, b));
	glm::vec3 ar = a[0] - glm::vec3(0.0f);
	float dis = glm::dot(ar , nor);
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
