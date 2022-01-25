// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "RenameUtilityWidget.generated.h"

class UButton;
class UDetailsView;

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

protected:
	UPROPERTY(meta=(BindWidget))
	UButton* Button_RenameAssets = nullptr;

	UPROPERTY(meta=(BindWidget))
	UDetailsView* DetailsView_BatchRename = nullptr;

	const FString RenameMessage = "Renaming assets...";

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BatchRename")
	FString NewName = "";

	UFUNCTION(CallInEditor, BlueprintCallable)
	void BatchRename();

private:
	static void GenerateFileName(FString& Name);
	static void SaveToLogFile(const FString& Message, const FString& FileName);
	static void GetDate(FString& Date);
};
