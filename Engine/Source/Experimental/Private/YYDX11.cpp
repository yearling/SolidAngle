#include "YYUT.h"
#include "YYDX11.h"
#include "YYUTDXManager.h"
#include "YYUTHelper.h"
#include <iostream>
#include <functional>
#include "YYUTCamera.h"
#include "Canvas.h"
#include "RenderMesh.h"
#include "FbxImporter.h"
#include "Misc/FbxErrors.h"
#include "YFbxConverter.h"
using std::cout;
using std::endl;

extern "C" __declspec(dllimport) LRESULT WINAPI SendMessageW( HWND   hWnd,UINT   Msg,WPARAM wParam,LPARAM lParam);
DEFINE_LOG_CATEGORY(YYDX11);
#define LOCTEXT_NAMESPACE "FbxMainImport"
using namespace UnFbx;

class TestSerialize
{
public:
	TestSerialize()
		:Version(0) {};
	int32 Version;
	TArray<int> Data;
	void Serialize(FArchive& Ar)
	{
		Ar << Version;
		Ar << Data;
	}
};
void DX11Demo::Initial()
{
	FPlatformTime::InitTiming();
	YYUTApplication::Initial();
	int default_x = 1920;
	int default_y = 1080;
	YYUTApplication::WindowCreate(default_x, default_y, _T("Render Mesh Demo"));
	m_width = default_x;
	m_height = default_y;
	YYUTDXManager::GetInstance().Init(m_spMainWindow->m_hWnd);
	YYUTDXManager::GetInstance().ReSize(m_width, m_height);



	/*FArchive* FileWriter = IFileManager::Get().CreateFileWriter(TEXT("YYTest.yy"));
	if (FileWriter)
	{
		TestOne.Serialize(*FileWriter);
	}
	delete FileWriter;

	FArchive* FileReader = IFileManager::Get().CreateFileReader(TEXT("YYTest.yy"));
	TestSerialize TestNewOne;
	if (FileReader)
	{
		TestNewOne.Serialize(*FileReader);
	}
	delete FileReader;*/
	//YYUTTimer::GetInstance().Start();
	XMFLOAT3 eye(40.0f, 40.0f, -40.0f);
	XMFLOAT3 lookat(0.0f, 0.0f, 0.0f);
	FVector eyeF(40.0f, 40.0f, -40.0f);
	FVector lookatF(0.0f, 0.0f, 0.0f);
	m_pCamera = new FirstPersionCamera();
	m_pLightCamera = new FirstPersionCamera();
	m_pCamera->SetViewParam(eyeF, lookatF);
	m_pCamera->SetProjParam(PI / 4, m_width / (float)m_height, 1.0f, 1000.0f);
	FVector lightEyeF(-320, 300, -220.3f);
	FVector ligntLookatF(0.0f, 0.0f, 0.0f);
	m_pLightCamera->SetViewParam(lightEyeF, ligntLookatF);
	m_pLightCamera->SetProjParam(PI / 4, 1, 50.0f, 300.0f);
	m_pLightCamera->FrameMove(0); 

	m_pSceneRender = std::make_shared<RenderScene>();
	m_pSceneRender->SetScreenWidthHeigth(default_x, default_y);

	m_pSceneRender->Init();
	// Draw ground grid
	GCanvas->Init();
	m_pCurrentCamera = m_pCamera;
	m_LastFrameTime = FPlatformTime::Seconds();
	m_bInit = true;

	EFBXImportType MeshTypeToImport;
	EFBXImportType OriginalImportType;
	FFbxImporter* FbxImporter = UnFbx::FFbxImporter::GetInstance();
	FFbxLoggerSetter Logger(FbxImporter);
	//FString FileToImport = TEXT("D:/wolf/Wolf_UDK.fbx");
	//FString FileToImport = TEXT("D:/wolf/Wolf_static.fbx");
	//FString FileToImport = TEXT("D:/wolf/humanoid.fbx");
	//FString FileToImport = TEXT("G:\\测试用FBX文件\\身体分多个模型的骨骼动画\\out.fbx");
	//FString FileToImport = TEXT("G:\\测试用FBX文件\\挂载武器的骨骼动画2\\attack.FBX");
	FString FileToImport = TEXT("C:/Users/yy/Desktop/fbxtest/lod/smoothgroup.FBX");
	int32 ImportType = FbxImporter->GetImportType(FileToImport);
	//int32 ImportType = 1;
	if (ImportType == -1)
	{
		FbxImporter->AddTokenizedErrorMessage(FTokenizedMessage::Create(EMessageSeverity::Warning, LOCTEXT("NoImportTypeDetected", "Can't detect import type. No mesh is found or animation track.")), FFbxErrors::Generic_CannotDetectImportType);
		m_bInit = false;
	}
	else 
	{
		MeshTypeToImport = EFBXImportType(ImportType);
		OriginalImportType = EFBXImportType(ImportType);
	}
	UnFbx::FBXImportOptions* ImportOptions = FbxImporter->GetImportOptions();

	// Set import parameters
	ImportOptions->bImportScene = false;
	ImportOptions->bImportMaterials = true;
	ImportOptions->bInvertNormalMap = false;
	ImportOptions->bImportTextures = true;
	ImportOptions->bImportLOD = false;
	ImportOptions->bUsedAsFullName = true;
	ImportOptions->bConvertScene = true;
	ImportOptions->bForceFrontXAxis = false;
	ImportOptions->bConvertSceneUnit = false;
	ImportOptions->bRemoveNameSpace = false;
	ImportOptions->ImportTranslation = FVector(0.0, 0.0, 0.0);
	ImportOptions->ImportRotation = FRotator(0.0, 0.0, 0.0);
	ImportOptions->ImportUniformScale = 1.0f;
	ImportOptions->NormalImportMethod = FBXNIM_ComputeNormals;
	//ImportOptions->NormalGenerationMethod = EFBXNormalGenerationMethod::MikkTSpace;
	ImportOptions->NormalGenerationMethod = EFBXNormalGenerationMethod::BuiltIn;
	ImportOptions->bTransformVertexToAbsolute = true;
	ImportOptions->bBakePivotInVertex = false;
	ImportOptions->bCombineToSingle = true;
	ImportOptions->VertexColorImportOption = EVertexColorImportOption::Ignore;
	ImportOptions->VertexOverrideColor = FColor(255, 255, 255, 255);
	ImportOptions->bRemoveDegenerates = true;
	ImportOptions->bBuildAdjacencyBuffer = true;
	ImportOptions->bBuildReversedIndexBuffer = true;
	ImportOptions->bGenerateLightmapUVs = true;
	ImportOptions->bOneConvexHullPerUCX = true;
	ImportOptions->bAutoGenerateCollision = true;
	ImportOptions->StaticMeshLODGroup = FName("None");
	ImportOptions->bImportStaticMeshLODs = false;
	ImportOptions->BaseMaterial = nullptr;
	ImportOptions->bImportMorph = false;
	ImportOptions->bImportAnimations = true;
	ImportOptions->bUpdateSkeletonReferencePose = false;
	ImportOptions->bResample = true;
	ImportOptions->bImportRigidMesh = false;
	ImportOptions->bUseT0AsRefPose = false;
	ImportOptions->bPreserveSmoothingGroups = true;
	ImportOptions->bKeepOverlappingVertices = false;
	ImportOptions->bImportMeshesInBoneHierarchy = true;
	ImportOptions->bCreatePhysicsAsset = true;
	ImportOptions->bImportSkeletalMeshLODs = false;
	ImportOptions->AnimationLengthImportType = FBXALIT_ExportedTime;
	ImportOptions->AnimationRange = FIntPoint(0, 0);
	ImportOptions->AnimationName = TEXT("");
	ImportOptions->bPreserveLocalTransform = false;
	ImportOptions->bDeleteExistingMorphTargetCurves = false;
	ImportOptions->bImportCustomAttribute = true;
	ImportOptions->bSetMaterialDriveParameterOnCustomAttribute = false;
	ImportOptions->bRemoveRedundantKeys = true;
	ImportOptions->bDoNotImportCurveWithZero = true;
	ImportOptions->MaterialCurveSuffixes.Reset();
	ImportOptions->MaterialCurveSuffixes.Add(TEXT("_mat"));
	ImportOptions->MaterialBasePath = FName("None");
//#if 0
	//FbxImporter->MainImport(FileToImport, EFBXImportType::FBXIT_SkeletalMesh);
	ImportResultPackage ImportResult =  FbxImporter->MainInportTest(FileToImport, EFBXImportType::FBXIT_StaticMesh);
	//ImportResultPackage ImportResult =  FbxImporter->MainInportTest(FileToImport, EFBXImportType::FBXIT_SkeletalMesh);
	if(ImportResult.SkeletalMesh!= nullptr)
	{ 
		m_pSceneRender->RegisterSkeletalMesh(ImportResult.SkeletalMesh,ImportResult.AnimSequence[0]);
		m_pSceneRender->PlayAnimation(ImportResult.AnimSequence[0]);
	}
	else if (ImportResult.StaticMeshes.Num())
	{
		for (UStaticMesh* pMesh : ImportResult.StaticMeshes)
		{
			m_pSceneRender->RegisterStaticMesh(pMesh);
	/*		FString NewFileName = pMesh->GetName() + "yy.yyStatic";
			TUniquePtr<FArchive> FileWriter(IFileManager::Get().CreateFileWriter(*NewFileName));
			if (FileWriter)
			{
				pMesh->Serialize(*FileWriter);
			}*/
		}
		
		
	}
//#else
	TUniquePtr<YFbxConverter>  FbxConverter = MakeUnique<YFbxConverter>();
	//FbxConverter->Init(TEXT("C:/Users/yy/Desktop/fbx/dummywithlod.FBX"));
	//if (FbxConverter->Init(TEXT("C:/Users/yy/Desktop/fbx/multiUVs/box2uv.FBX")))
	//if (FbxConverter->Init(TEXT("C:/Users/yy/Desktop/fbx/one_mesh_with_multi_material/box.FBX")))
	if (FbxConverter->Init(TEXT("C:/Users/yy/Desktop/fbxtest/lod/smoothgroup.FBX")))
	{
		TUniquePtr<YFBXImportOptions> ImportOptionsy = MakeUnique< YFBXImportOptions>();
		FbxConverter->Import(std::move(ImportOptionsy));
	}
	
	/*TUniquePtr<FArchive>FileReader(IFileManager::Get().CreateFileReader(TEXT("yy.yyStatic")));
	UStaticMesh* pSerialMesh = new UStaticMesh();
	if (FileReader)
	{
		pSerialMesh->Serialize(*FileReader);
	}
	m_pSceneRender->RegisterStaticMesh(pSerialMesh);*/
//#endif
	m_pSceneRender->AllocResource();
}


void DX11Demo::Exit()
{

}

LRESULT DX11Demo::MyProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) throw()
{
	switch (message)
	{

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
		{
			SendMessageW(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		}
		//不能用break，要向下传递
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
	case WM_RBUTTONUP:
	case WM_MOUSEWHEEL:
	{
		int x = (short)LOWORD(lParam);
		int y = (short)HIWORD(lParam);
		//m_vVelocity=XMFLOAT3(0,0,0);
		if (message == WM_KEYDOWN)
		{
			UINT key = (UINT)wParam;
			switch (key)
			{
			case 'W':
				m_vVelocity.z = 20.0f;
				m_vVelocityF.Z = 20.0f;
				break;
			case 'S':
				m_vVelocity.z = -20.0f;
				m_vVelocityF.Z = -20.0f;
				break;
			case 'A':
				m_vVelocity.x = -20.0f;
				m_vVelocityF.X = -20.0f;
				break;
			case 'D':
				m_vVelocity.x = 20.0f;
				m_vVelocityF.X = 20.0f;
				break;
			case 'R':
			{
				if (!m_bSwitchButtonDown)
				{
					m_bSwitchButtonDown = true;
					SwichCamera();
				}
			}
			break;

			default:
				break;
			}
		}
		if (message == WM_KEYUP)
		{
			UINT key = (UINT)wParam;
			switch (key)
			{
			case 'W':
				m_vVelocity.z = 0.0f;
				m_vVelocityF.Z = 0.0f;
				break;
			case 'S':
				m_vVelocity.z = 0.0f;
				m_vVelocityF.Z = 0.0f;
				break;
			case 'A':
				m_vVelocity.x = 0.0f;
				m_vVelocityF.X = 0.0f;
				break;
			case 'D':
				m_vVelocity.x = 0.0f;
				m_vVelocityF.X = 0.0f;
				break;
			case 'R':
			{
				m_bSwitchButtonDown = false;
			}
			break;
			case 'Y':
			{
				m_bShowColorLayer = false;
			}
			break;
			default:
				break;
			}
		}
		if (message == WM_LBUTTONDOWN)
		{
			m_bMouseLDown = true;
			m_LastMousePosition.x = x;
			m_LastMousePosition.y = y;
			m_fPitchOrigin = m_pCurrentCamera->GetPitch();
			m_fYawOrigin = m_pCurrentCamera->GetYaw();
		}
		if (message == WM_RBUTTONDOWN)
		{
			m_bMouseRDown = true;
			m_LastMousePosition.x = x;
			m_LastMousePosition.y = y;
			m_fPitchOrigin = m_pLightCamera->GetPitch();
			m_fYawOrigin = m_pLightCamera->GetYaw();
		}
		if (message == WM_MBUTTONDOWN)
		{
			m_bMouseMDown = true;
		}
		if (message == WM_LBUTTONUP)
		{
			m_bMouseLDown = false;
			m_vMouseDelta.x = 0.0f;
			m_vMouseDelta.y = 0.0f;
		}
		if (message == WM_RBUTTONUP)
		{
			m_bMouseRDown = false;
			m_vMouseDelta.x = 0.0f;
			m_vMouseDelta.y = 0.0f;
		}
		if (message == WM_MBUTTONUP)
		{
			m_bMouseMDown = false;
		}
	}
	break;
	case WM_MOUSEMOVE:
	{
		int x = (short)LOWORD(lParam);
		int y = (short)HIWORD(lParam);
		if (m_bMouseLDown || m_bMouseRDown)
		{
			m_vMouseDelta.x = float(x - m_LastMousePosition.x);
			m_vMouseDelta.y = float(y - m_LastMousePosition.y);
		}
		break;
	}
	case WM_SIZE:
	{
		m_width = LOWORD(lParam);
		m_height = HIWORD(lParam);
		if (YYUTDXManager::GetInstance().IsInitialized())
		{
			switch (wParam)
			{
			case SIZE_MINIMIZED:
			{
				OnMinimize();
				YYUTDXManager::GetInstance().PauseAll(true);
				break;
			}
			case SIZE_MAXIMIZED:
			{
				OnResize();
				YYUTDXManager::GetInstance().PauseAll(false);
				break;
			}
			case SIZE_RESTORED:
			{
				if (!m_bResize)
				{
					OnResize();
				}
				break;
			}
			default:
				break;
			}
		}
	}
	case WM_ENTERSIZEMOVE:
	{
		m_bResize = true;
		//YYUTDXManager::GetInstance().PauseAll(true);
		break;
	}
	case WM_EXITSIZEMOVE:
	{
		m_bResize = false;
		//YYUTDXManager::GetInstance().PauseAll(false);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;
		//自定义的全屏函数，必须在有MSGLOOP的线程下工作。
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void DX11Demo::GameMain()
{
	//YYUTDXManager::GetInstance().Tick(0);
}



int DX11Demo::Run()
{
	MSG msg;
	double elapseTime = 0.0f;
	while (1)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (WM_QUIT == msg.message)
				return static_cast<int>(msg.wParam);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		//elapseTime = FPlatformTime::ToSeconds64(FPlatformTime::Cycles64() - m_LastTickCycles);
		elapseTime = FPlatformTime::Seconds() - m_LastFrameTime;
		m_LastFrameTime = FPlatformTime::Seconds();
		Update(elapseTime);
		Render(elapseTime);
		if (FPlatformTime::Seconds() - m_LastSecond > 1.0)
		{
			m_FPS =( (double)m_LastSecondFrames) / (FPlatformTime::Seconds() - m_LastSecond);
			m_LastSecond = FPlatformTime::Seconds();
			m_LastSecondFrames = 0;
		}
		else
		{
			m_LastSecondFrames++;
		}
	}
	return static_cast<int>(msg.wParam);
}

void DX11Demo::OnResize()
{
	//YYUTDXManager::GetInstance().ReSize(m_width, m_height);
}

void DX11Demo::OnMinimize(void)
{
	//YYUTDXManager::GetInstance().PauseAll(true);
}

float DX11Demo::GetFPS() const
{
	return m_FPS;
}

DX11Demo::DX11Demo() :
	m_bResize(false),
	m_width(0),
	m_height(0),
	m_bMouseLDown(false),
	m_bMouseMDown(false),
	m_bMouseRDown(false),
	m_fYawOrigin(0.0f),
	m_fPitchOrigin(0.0f),
	m_bSwitchButtonDown(false),
	m_pCurrentCamera(nullptr),
	m_pCamera(nullptr),
	m_pLightCamera(nullptr),
	m_bInit(false),
	m_bShowColorLayer(false),
	m_FPS(0.0f),
	m_LastSecondFrames(0)
{
	//m_vVelocity = XMFLOAT3(0, 0, 0);
}

DX11Demo::~DX11Demo()
{

}

void DX11Demo::Update(float ElapseTime)
{
	//m_Camera.AddPitchYaw(m_vMouseDelta.y*ElapseTime,m_vMouseDelta.x*ElapseTime);
	if (!m_bInit)
		return;
	//std::cout << "Update::Velocity: " << m_vVelocity.x << "  " << m_vVelocity.z << std::endl;
	m_pCurrentCamera->SetVelocity(m_vVelocityF);
	//cout << " x: " << vEyePos.x << " y: " << vEyePos.y << " z: " << vEyePos.z << endl;
	if (m_bMouseLDown && !m_bMouseRDown)
	{
		float fpitch = m_fPitchOrigin + m_vMouseDelta.y*0.005f;
		float fyaw = m_fYawOrigin + m_vMouseDelta.x*0.005f;
		m_pCurrentCamera->SetPitch(fpitch);
		m_pCurrentCamera->SetYaw(fyaw);
		m_pCurrentCamera->FrameMove(ElapseTime);
	}
	else if (m_bMouseRDown && !m_bMouseLDown && m_pCurrentCamera != m_pLightCamera)
	{
		float fpitch = m_fPitchOrigin + m_vMouseDelta.y*0.005f;
		float fyaw = m_fYawOrigin + m_vMouseDelta.x*0.005f;
		m_pLightCamera->SetPitch(fpitch);
		m_pLightCamera->SetYaw(fyaw);
		m_pLightCamera->SetVelocity(FVector(0.0f, 0.0f, 0.0f));
		m_pLightCamera->FrameMove(ElapseTime);
	}
	else
		m_pCurrentCamera->FrameMove(ElapseTime);
	//m_pShadow->Update(ElapseTime);
}

void DX11Demo::Render(float ElapseTime)
{
	TSharedRef<FRenderInfo> pRenderInfo = MakeShared<FRenderInfo>();
	pRenderInfo->RenderCameraInfo.View = m_pCamera->GetView();
	pRenderInfo->RenderCameraInfo.Projection = m_pCamera->GetProject();
	pRenderInfo->RenderCameraInfo.ViewProjection = m_pCamera->GetViewProject();
	pRenderInfo->RenderCameraInfo.ViewProjectionInv = m_pCamera->GetViewProjInv();
	pRenderInfo->SceneInfo.MainLightDir = m_pLightCamera->GetDir();
	pRenderInfo->TickTime = ElapseTime;
	pRenderInfo->FPS = GetFPS();
	YYUTDXManager::GetInstance().AddRenderEvent([this, pRenderInfo]() {m_pSceneRender->Render(pRenderInfo); });
	YYUTDXManager::GetInstance().Render();
}

void DX11Demo::SwichCamera()
{
	if (m_pCurrentCamera == m_pCamera)
	{
		m_pCurrentCamera = m_pLightCamera;
	}
	else
	{
		m_pCurrentCamera = m_pCamera;
	}
	//m_pShadow->SwitchCamera();
}

#undef LOCTEXT_NAMESPACE