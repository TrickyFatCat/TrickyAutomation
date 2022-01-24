// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "RenameUtilityWidget.generated.h"

class UButton;
class UDetailsView;

/**
 * 
 */
UCLASS()
class TRICKYAUTOMATION_API URenameUtilityWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()

public:
	virtual void NativePreConstruct() override;

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_RenameAssets = nullptr;

	UPROPERTY(meta=(BindWidget))
	UDetailsView* DetailsView_BatchRename = nullptr;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BatchRename")
	FString NewName = "";
	
	UFUNCTION(CallInEditor, BlueprintCallable)
	void BatchRename();
};
