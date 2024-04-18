// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMainMenu.h"

#include "UObject/ConstructorHelpers.h"   // para lo del constructor

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "ServerRow.h"


UMyMainMenu::UMyMainMenu(const FObjectInitializer & ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));  // FClassFinder solo puede ser utilizada desde un constructor
    if(!ensure(ServerRowBPClass.Class != nullptr)) return;

    ServerRowClass = ServerRowBPClass.Class;
}


bool UMyMainMenu::Initialize()
{
    bool Success = Super::Initialize();
    if(!Success) return false;

    if(!ensure(HostButton!=nullptr)) return false;
    HostButton->OnClicked.AddDynamic(this, &UMyMainMenu::OpenHostMenu);

    if(!ensure(CancelHostMenuButton!=nullptr)) return false;
    CancelHostMenuButton->OnClicked.AddDynamic(this, &UMyMainMenu::OpenMainMenu);

    if(!ensure(ConfirmHostMenuButton!=nullptr)) return false;
    ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UMyMainMenu::HostServer);

    if(!ensure(JoinButton!=nullptr)) return false;
    JoinButton->OnClicked.AddDynamic(this, &UMyMainMenu::OpenJoinMenu);

    if(!ensure(QuitButton!=nullptr)) return false;
    QuitButton->OnClicked.AddDynamic(this, &UMyMainMenu::QuitPressed);

    if(!ensure(CancelJoinMenuButton!=nullptr)) return false;
    CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMyMainMenu::OpenMainMenu);

    if(!ensure(ConfirmJoinMenuButton!=nullptr)) return false;
    ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UMyMainMenu::JoinServer);

    return true;
}

void UMyMainMenu::OpenHostMenu()
{
    if (!ensure (MenuSwitcher != nullptr)) return;
    if (!ensure (HostMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMyMainMenu::HostServer()
{
    if (CurrentMenuInterface != nullptr)  // hacemos uso de la interfaz porque así podemos separar el menú del juego y llamar a este menú en distintas instancias del juego por ejemplo. Sin la necesidad de tener una referencia directa al game instance.
    {
        FString ServerName = ServerHostName->Text.ToString();
        CurrentMenuInterface->Host(ServerName);   // dependency inversion - me conecta con el host de puzzleplatformsgameinstance ya que se lo inyectamos en setmenuinterface
    }

}

void UMyMainMenu::SetServerList(TArray<FServerData> ServerNames)
{     
    UWorld* World = this->GetWorld();
    if(!ensure(World != nullptr)) return;

    ServerList->ClearChildren();
    
    uint32 i = 0;
    for (const FServerData& ServerData : ServerNames)
    {
        UServerRow* Row = CreateWidget<UServerRow>(World, ServerRowClass);   // no es solo un UUserWidget (especificamos) (seteamos previamente en el blueprint como parent class el userverrow)
        if(!ensure(Row != nullptr)) return;

        Row->ServerName->SetText(FText::FromString(ServerData.Name));
        Row->HostUser->SetText(FText::FromString(ServerData.HostUsername));
        FString FractionText = FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers);  // esto es un to string (interpretar int como string)
        Row->ConnectionFraction->SetText(FText::FromString(FractionText));
        Row->Setup(this, i);
        ++i;
        ServerList->AddChild(Row);
    }
}

void UMyMainMenu::SelectIndex(uint32 Index)
{
    SelectedIndex = Index;   // seteamos el TOptional
    UpdateChildren();
}

void UMyMainMenu::UpdateChildren()
{
    for (int32 i = 0; i < ServerList->GetChildrenCount(); i++ )
    {
        auto Row = Cast<UServerRow>(ServerList->GetChildAt(i));  //tipo UServerRow*, ponemos auto en vez de esto para no repetir el tipo que ya está dentro del cast pero en verdad da igual
        if (Row != nullptr)
        {
            Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);  // short circuit operator - evalúa primero lo de la izquierda y luego lo de la derecha (sale antes de evaluar lo de la derecha si no cumple la condición (si fuera un OR saldría antes de tiempo si ya cumple la condición))
        }
    }  
}

void UMyMainMenu::JoinServer()
{
    if (SelectedIndex.IsSet() && CurrentMenuInterface != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected index %d"), SelectedIndex.GetValue());
        CurrentMenuInterface->Join(SelectedIndex.GetValue());   
    } 
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("Selected index not set"));
    }
}


void UMyMainMenu::OpenJoinMenu()
{
    if (!ensure (MenuSwitcher != nullptr)) return;
    if (!ensure (JoinMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(JoinMenu);
    if (CurrentMenuInterface != nullptr)
    {
        CurrentMenuInterface->RefreshServerList();
    }
}

void UMyMainMenu::OpenMainMenu()
{
    if (!ensure (MenuSwitcher != nullptr)) return;
    if (!ensure (MainMenu != nullptr)) return;
    MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMyMainMenu::QuitPressed()   // esto lo ponemos directamente en vez de a través de una interfaz ya que la manera de salir del juego no cambia dependiendo del juego (ejecutar comando). Si queremos hacer algo específico dependiendo de la instancia pues sí habría que sacarlo.
{
    UWorld* World = GetWorld();
    if (!ensure (World != nullptr)) return;

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!ensure (PlayerController != nullptr)) return;

    PlayerController->ConsoleCommand("Quit");
}

