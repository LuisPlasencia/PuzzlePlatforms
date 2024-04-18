// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMenuWidget.h"


void UMyMenuWidget::Setup()
{
    this->AddToViewport();

    UWorld* World = GetWorld();
    if (!ensure (World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!ensure (PlayerController != nullptr)) return;

    FInputModeUIOnly InputModeData;
    InputModeData.SetWidgetToFocus(this->TakeWidget());   // para que el menu sea lo único interactuable
    InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);  // para poder mover el raton 

    PlayerController->SetInputMode(InputModeData);

    PlayerController->bShowMouseCursor = true;   // para poder ver el raton
}


void UMyMenuWidget::TearDown()
{
    this->RemoveFromViewport();

    UWorld* World = GetWorld();
    if (!ensure (World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!ensure (PlayerController != nullptr)) return;

    FInputModeGameOnly InputModeData;
    PlayerController->SetInputMode(InputModeData);

    PlayerController->bShowMouseCursor = false;  
}



void UMyMenuWidget::SetMenuInterface(IMenuInterface* MenuInterface)
{
    this->CurrentMenuInterface = MenuInterface;
}


