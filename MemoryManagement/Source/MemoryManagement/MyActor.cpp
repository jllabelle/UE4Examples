#include "MyActor.h"

#include "Components/SceneComponent.h"
#include "EngineUtils.h"
#include "MyComponent1.h"

DECLARE_LOG_CATEGORY_EXTERN(MyActor, Log, All);
DEFINE_LOG_CATEGORY(MyActor);

static FName DeleteMeTag = TEXT("DeleteMe");

AMyActor::AMyActor()
{
	UE_LOG(MyActor, Log, TEXT("AMyActor::AMyActor"));

	PrimaryActorTick.bCanEverTick = true;

	UPropertyDefaultSubobject = CreateDefaultSubobject<UMyComponent1>(TEXT("UPropertyDefaultSubobject"));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("My Root"));
}

static void CheckPointerInternal(UObject* Pointer, FString Name)
{
	if (Pointer == nullptr || !Pointer->IsValidLowLevel())
	{
		UE_LOG(MyActor, Error, TEXT("%s going away %p"), *Name, Pointer);
	}
}

#define CheckPointer(Pointer) CheckPointerInternal(Pointer, #Pointer)

void AMyActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UE_LOG(MyActor, Log, TEXT("AMyActor::Tick %f, %s"), DeltaSeconds, *GetName());

	CheckPointer(UPropertyDefaultSubobject);
	CheckPointer(UPropertyNewObject);
	CheckPointer(Naked);
	CheckPointer(Weak.Get());
}


void AMyActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UE_LOG(MyActor, Log, TEXT("AMyActor::PostInitializeComponents: %s"), *GetName());

	if (ActorHasTag(DeleteMeTag))
	{
		UPropertyNewObject = NewObject<UMyComponent1>(this, TEXT("UPropertyNewObject"));
		UPropertyNewObject->RegisterComponent();

		Naked = NewObject<UMyComponent1>(this, TEXT("Naked"));
		Naked->RegisterComponent();

		Weak = NewObject<UMyComponent1>(this, TEXT("Weak"));
		Weak->RegisterComponent();
	}
}


void AMyActor::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(MyActor, Log, TEXT("AMyActor::BeginPlay: %s"), *GetName());

	AMyActor* ActorToCopy = nullptr;
	for (TActorIterator<AMyActor> It(GetWorld()); It; ++It)
	{
		if (It->ActorHasTag(DeleteMeTag))
		{
			ActorToCopy = *It;
		}
	}


	if (ActorToCopy)
	{
		UE_LOG(MyActor, Log, TEXT("AMyActor::BeginPlay Copying pointers from %s to %s"), *ActorToCopy->GetName(), *GetName());
		UPropertyDefaultSubobject = ActorToCopy->UPropertyDefaultSubobject;
		UPropertyNewObject = ActorToCopy->UPropertyNewObject;
		Naked = ActorToCopy->Naked;
		Weak = ActorToCopy->Weak;
	}
}

