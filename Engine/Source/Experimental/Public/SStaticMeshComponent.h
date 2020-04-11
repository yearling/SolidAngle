#include "Core.h"
#include "SComponent.h"
#include "SStaticMesh.h"
class SStaticMeshComponent :public SSceneComponent
{
public:
	SStaticMeshComponent()
	{
		ComponentType = EComponentType::StaticMeshComponent;
	}
	TRefCountPtr<SStaticMesh> StaticMesh;
};
