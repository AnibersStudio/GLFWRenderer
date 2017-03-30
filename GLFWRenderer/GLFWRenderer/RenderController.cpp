#include "RenderController.h"

RenderController::RenderController(MeshManager & mm) : meshmanager(mm)
{
	
}

void RenderController::Draw(RenderContext context)
{
	RenderPrepare();
	






	oldcontext = context;
}

void RenderController::RenderPrepare()
{
	framedata.Clear();
	meshmanager.AppendPosition(MeshManager::Opace, framedata.OpacePosition);
	meshmanager.AppendPosition(MeshManager::FullTrans || MeshManager::SemiTrans, framedata.TransPosition);
}
