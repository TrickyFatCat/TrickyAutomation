// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "WidgetBlueprint.h"
#include "Animation/AimOffsetBlendSpace.h"
#include "Animation/AimOffsetBlendSpace1D.h"
#include "Animation/BlendSpace.h"
#include "Animation/BlendSpace1D.h"
#include "Engine/Blueprint.h"
#include "Sound/SoundWave.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/Skeleton.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "Particles/ParticleSystem.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "Engine/Font.h"
#include "Engine/FontFace.h"
#include "RenameUtilityWidget.generated.h"

class UButton;
class USinglePropertyView;
class UDetailsView;
class UComboBoxString;

DECLARE_LOG_CATEGORY_CLASS(LogRenameUtility, Display, All)

/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API URenameUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

#pragma region RenameSelectedAssets
	
protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_RenameAssets = nullptr;

	UPROPERTY(meta=(BindWidget))
	USinglePropertyView* PropertyView_NewName = nullptr;

	UPROPERTY(meta=(BindWidget))
	UComboBoxString* ComboBox_SuffixRename = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BatchRename")
	FString NewName = "";

	UFUNCTION(CallInEditor)
	void BatchRename();
	
private:
	const FString RenameMessage = "Renaming assets...";
#pragma endregion

#pragma region AddPrefixes

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_AddPrefix = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="AddPrefix")
	TMap<UClass*, FString> PrefixesMap = {
		{UBlueprint::StaticClass(), "BP"},
		{UStaticMesh::StaticClass(), "SM"},
		{USkeletalMesh::StaticClass(), "SKM"},
		{USkeleton::StaticClass(), "SKEL"},
		{USoundWave::StaticClass(), "SW"},
		{USoundCue::StaticClass(), "SCUE"},
		{USoundClass::StaticClass(), "SC"},
		{UAnimBlueprint::StaticClass(), "ABP"},
		{UAnimSequence::StaticClass(), "AS"},
		{UAnimMontage::StaticClass(), "AM"},
		{UBlendSpace::StaticClass(), "BS"},
		{UBlendSpace1D::StaticClass(), "BS"},
		{UAimOffsetBlendSpace::StaticClass(), "ABS"},
		{UAimOffsetBlendSpace1D::StaticClass(), "ABS"},
		{UMaterial::StaticClass(), "M"},
		{UMaterialInstance::StaticClass(), "MI"},
		{UMaterialFunction::StaticClass(), "MF"},
		{UTexture2D::StaticClass(), "T"},
		{UWidgetBlueprint::StaticClass(), "WBP"},
		{UBlueprintFunctionLibrary::StaticClass(), "BPFL"},
		{UBlackboardData::StaticClass(), "BB"},
		{UBehaviorTree::StaticClass(), "BT"},
		{UEnvQuery::StaticClass(), "EQ"},
		{UParticleSystem::StaticClass(), "PS"},
		{UPhysicalMaterial::StaticClass(), "PM"},
		{UPhysicsAsset::StaticClass(), "PA"},
		{UFont::StaticClass(), "F"},
		{UFontFace::StaticClass(), "FF"},
		{UEnum::StaticClass(), "E"},
		{UStruct::StaticClass(), "F"}
	};

	UFUNCTION(CallInEditor)
	void AddPrefix();

private:
	const FString AddPrefixMessage = "Adding prefixes...";
	bool PrefixesMapIsEmpty() const;
	static void GeneratePrefix(FString& Prefix);

#pragma endregion

#pragma region AddSuffixes

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_AddSuffix = nullptr;

	UPROPERTY(meta=(BindWidget))
	UComboBoxString* ComboBox_Suffixes = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AddSuffix")
	TArray<FString> SuffixesArray = {
		"N",
		"D",
		"R"
	};

	UFUNCTION(CallInEditor)
	void AddSuffix();

private:
	const FString AddSuffixMessage = "Adding suffixes...";
	
#pragma endregion 

private:
	static void GenerateFileName(FString& Name);
	static void SaveToLogFile(const FString& Message, const FString& FileName);
	static void GetDate(FString& Date);
};
