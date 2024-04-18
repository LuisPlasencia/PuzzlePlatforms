// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MyMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UMyMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetMenuInterface(IMenuInterface* MenuInterface);

	void Setup();

	void TearDown();

 protected: //anything that inherits from MyMenuWidget has access to this:
	IMenuInterface* CurrentMenuInterface;   // incluyo la interfaz y la uso (no la implemento), pero la setea el gameInstance (dependency inversion)
	
};
