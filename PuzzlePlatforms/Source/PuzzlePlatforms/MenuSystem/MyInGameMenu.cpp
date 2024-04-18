// Fill out your copyright notice in the Description page of Project Settings.


#include "MyInGameMenu.h"
#include "Components/Button.h"

bool UMyInGameMenu::Initialize()
{
    bool Success = Super::Initialize();
    if(!Success) return false;

    if(!ensure(CancelButton!=nullptr)) return false;
    CancelButton->OnClicked.AddDynamic(this, &UMyInGameMenu::CancelPressed);  // llamar a teardown del padre para quitar el menú es buena idea pero no funcionará si no es una UFUNCTION. Como teardown no tiene sentido que sea UFUNCTION creamos otra funcion que sí lo es pero que solo llama a teardown para la buena práctica.

    if(!ensure(QuitButton!=nullptr)) return false;
    QuitButton->OnClicked.AddDynamic(this, &UMyInGameMenu::QuitPressed); 

    return true;
}

void UMyInGameMenu::CancelPressed()
{
    TearDown();
}

void UMyInGameMenu::QuitPressed()
{
    if (CurrentMenuInterface != nullptr)
    {
        TearDown();
        CurrentMenuInterface->LoadMainMenu();   
    }
}