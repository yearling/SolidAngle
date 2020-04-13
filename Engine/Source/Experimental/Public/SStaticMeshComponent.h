#include "Core.h"
#include "SComponent.h"
#include "SStaticMesh.h"
DECLARE_LOG_CATEGORY_EXTERN(logSStaticMeshCompoent, Log, All)
class SStaticMeshComponent :public SSceneComponent
{
public:
	SStaticMeshComponent()
	{
		ComponentType = EComponentType::StaticMeshComponent;
	}
	TRefCountPtr<SStaticMesh> StaticMesh;

};
