// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

#include "PlatformTrigger.h"
#include "MenuSystem/MyMainMenu.h"
#include "MenuSystem/MyMenuWidget.h"

const static FName SESSION_NAME = NAME_GameSession; // uso NAME_GameSession porque es la default en el null subsystem y no cambia entre versiones y es "Game". Realmente podemos poner el nombre que queramos pero de esta forma podemos testear con el nullSubsystem la variable Session.NumOpenPublicConnections de forma adecuada.
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

// nos comunicamos con esta clase a través de los menús haciendo uso de la interfaz. El constructor de esta clase se llama en el editor y cuando iniciamos el juego y el init solo al iniciar el juego. Esta clase se establece en preferencias->default game instance.
// esta clase es una buena clase para spawnear y crear UI
// solo hay una instancia de esta clase cuando el juego está en ejecución independientemente del nivel o mapa (no depende del nivel así que es perfecto para la lógica de cambiar entre niveles) 

// constructor se llama en el editor y cuando iniciamos el juego
UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
    ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));  // FClassFinder solo puede ser utilizada desde un constructor
    if(!ensure(MenuBPClass.Class != nullptr)) return;

    MenuClass = MenuBPClass.Class;

    ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));  // FClassFinder solo puede ser utilizada desde un constructor
    if(!ensure(InGameMenuBPClass.Class != nullptr)) return;

    InGameMenuClass = InGameMenuBPClass.Class;
}

//  el init se llama cuando iniciamos el juego
void UPuzzlePlatformsGameInstance::Init()
{
    // el online subsystem es una capa de abstraccion que nos ayuda a implementar un sistema (múltiples interfaces) online para distintas plataformas: steam, consolas, android, ios, etc.
    // podríamos poner como plataforma default steam (Config->DefaultEngine.ini) que es la que vamos a usar pero complica el testeo. Es mejor hacer la plataforma default null para testear el onlinesubsystem (tiene lo mínimo indispensable: local network etc). Luego la podemos cambiar por steam.
    // Ya me salía null por defecto así que no lo hice pero si se necesita añadir esto en defaultEngine.ini: 
    // [OnlineSubsystem]
    // DefaultPlatformService = NULL

    // OJO: en el editor solo nos aparecerá el NULL subsystem, si queremos usar el de steam debemos launchear el game aparte y tener steam abierto.
    // OJO: Hemos cambiado el subsystem a steam en vez de null porque ya hicimos el testing, para ello si que hay que especificar DefaultPlatformService = steam.
    // para añadir steam subsystem añadimos el plugin Online Subsystem Steam, el módulo "OnlineSubsystemSteam" en el fichero build.cs  y  ir al fichero Config/DefaultEngine.ini  y añadir [OnlineSubsystem] DefaultPlatformService=Steam aparte de más cosas incluidas en la documentación
    // en el fichero Config/DefaultEngine.ini tenemos por defecto el steamDevAppId=480 que es el appId de spacewar la cual es la única gratis  (si no, tenemos que pagar 100 euros para ser developers de steam y recibir otra AppId que podriamos utilizar)
    // el netdriver nos ahorra usar vpn (direct ip connection) ya que nos hace la conexión a través de steam (servicios)

    // Log LogOnline VeryVerbose . comando para  ver más logs ingame
    // si se traba el game launcheado aparte hacer ctr+c para que avance la ventana de logs
    
    // TIP: para desconectar steam online subsystem y usar null online subsystem añadir -nosteam al comando o poner benabled=false en el fichero Config/defaultEngine.ini 

    IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
    if(Subsystem != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
        SessionInterface = Subsystem->GetSessionInterface();  // podemos poner auto en vez del tipo si no estamos seguros
        if (SessionInterface.IsValid())  // sessioninterface no es un puntero simple y convencional sino un (TSharedPtr), así que comprobamos si apunta a un objeto así.  STACK VS HEAP
        // STACK y HEAP son segmentos de memoria. El stack se usa estructuradamente y el heap de forma más dinámica (dynamic memory allocation).
        // El STACK ocupa un slot de memoria (de arriba hacia abajo: añadimos por abajo) por cada función llamada y se va limpiando cuando se usan dichas funciones (clear the stack frame) de forma organizada y unfragmented (ordenado)
        // Para guardar las variables de cada jugador, por ejemplo, utilizamos el HEAP ya que dichas variables suelen ser generadas de forma dinámica a lo largo de la ejecución del juego, creando una estructura fragmentada en la que se ocupa y se libera memoria (messy).
        
        // Formas de liberar las variables del HEAP de memoria: 
        // 1. Reference Counting: las variables del HEAP son eliminadas (en el destructor) en función del número de referencias que tengan (punteros en memoria, Reference Counting) así se evitan memory leaks
        // La desventaja del Reference Counting está en las referencias cruzadas o circulares (jugador1 apunta a jugador2 y vicebersa), haciendo que nunca se liberen de memoria (memory leak). Además si algún destructor elimina de golpe muchas variables de memoria ya que almacena muchas referencias puede ralentizar el tick de nuestro juego.
        //
        // 2. Garbage Collection: lista de objetos y busco de arriba hacia abajo los objetos a los que no puedo alcanzar y los elimino.
        //
        // La manera de Unreal: 
        // 1. How do we reference count an object? 
        //      Use a TSharedPtr<AActor> en el Stack y apunta a un objeto del Heap. Construir un TSharedPtr aumenta el número de referencias del objeto al que apunta (mientras haya al menos un TSharedPtr apuntando a un objeto, este jamás será eliminado). 
        //      Mientras deleteamos objetos del Stack, puede que deleteemos también el TSharedPtr lo cual decrementará el número de referencias del objeto al que apunta.
        //      
        // 2. How does Unreal know what objects to scan for garbage collection?
        //      Todos los UObjects son parte del garbage collection set. Unreal empieza a rastrear desde el root set hacia abajo a través de los UProperty pointers de los UObject (un UProperty puede apuntar a otro objeto, en cuyo caso no se eliminaría dicho UObject).
        //      Cualquier UObject no encontrado en alguno de los UProperty pointers rastreados puede ser eliminado por el garbage collector.
        // 
        // 3. How does Unreal know which pointers it should follow?
        //      A través de UProperty.
        //
        // Ahora tiene sentido el TSharedPtr de arriba y es que al tratarse de una interfaz y no un UObject, queremos emplear un modo referenciar más fácil que un raw pointer, el cual hay que alocar y desalocarlo de memoria, lo cual es un coñazo.
        {
            SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);  // añadimos callback para que cuando llamemos a crear sesión y se complete la operación, hagamos algo (asíncrono)
            SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
            SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
            SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
            // OnCreateSessionCompleteDelegates es un Multi-Cast delegate, es decir, puedo enlazarlo a múltiples cosas, en este caso UObject es el mejor porque va a ser memory managed y garbage collected automáticamente.
            // OJO: Por qué usamos aquí AddUObject y no AddUFunction? A diferencia de los delegados de movingplatform, por ejemplo, aquí estamos usando un delegado TMulticastDelegate con dos parámetros. No es un delegate dinámico sino estático (no necesidad de UFunction en este caso sino funciones C++ normales así que me puedo ahorrar el UFUNCTION())
            // typedef TMulticastDelegate_TwoParams< void, FName, bool > FOnCreateSessionComplete  --- esto significa que retorna void y necesita dos parámetros: un FName y un bool
    
        }
    }
    else 
    {
        UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
    }

    if (GEngine != nullptr)
    {
        GEngine->OnNetworkFailure().AddUObject(this, &UPuzzlePlatformsGameInstance::OnNetworkFailure);  // nos  suscribimos a un evento directamente desde el objeto UEngine para tratar errores de network (por ejemplo: si el host cierra el juego y aun hay clientes dentro se les freezea la pantalla si no lo tratamos) - el formato del delegado (signature) se encuentra en Engine.h, podemos ir haciendole F12 a GEngine->OnNetworkFailure()
        // Esto viene de GEngine y no de SessionInterface porque la connection logic (una vez que estamos conectados a un server) ocurre a nivel de Engine, no a nivel de sessionManagement (el sessionmanagement nos conecta y desconecta pero una vez estamos conectados a un server es el engine el que pasa a gestionar la mayoría de los procesos)
    }
}

void UPuzzlePlatformsGameInstance::LoadMenu()
{

    // OJO: If we don't know the type of a widget that we want to point do just point to UWidget

    if (!ensure(MenuClass != nullptr)) return;
    MainMenu = CreateWidget<UMyMainMenu>(this, MenuClass);   // no es solo un UUserWidget (especificamos) (seteamos previamente en el blueprint como parent class el mymainmenu)
    if(!ensure(MainMenu != nullptr)) return;

    MainMenu->Setup();

    MainMenu->SetMenuInterface(this);

}

void UPuzzlePlatformsGameInstance::InGameLoadMenu()
{
    if (!ensure(InGameMenuClass != nullptr)) return;
    
    UMyMenuWidget* InGameMenu = CreateWidget<UMyMenuWidget>(this, InGameMenuClass);   // Es un UMyMenuWidget porque aunque el blueprint herede del InGameMenu, este a su vez hereda de UMyMenuWidget 
    if(!ensure(InGameMenu != nullptr)) return;

    InGameMenu->Setup();

    InGameMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::Host(FString ServerName)
{
    DesiredServerName = ServerName;

    if (SessionInterface.IsValid())
    {
        auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME); // auto si no estoy seguro del tipo y tampoco me importa porque solo me importa que sea nulo o no
        if (ExistingSession != nullptr)
        {
            SessionInterface->DestroySession(SESSION_NAME);
        }
        else 
        {
           CreateSession();
        }
    }
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
    if (Success)
    {
        CreateSession();
    }
}

void UPuzzlePlatformsGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
    LoadMainMenu(); // volvemos al menú principal si hay errores de conexión o dejamos de tener conexión con el servidor etc. (Si no se quedaría el juego freeze porque si somos el cliente y derepente desaparece el servidor nos quedamos en el limbo)
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
    if(SessionInterface.IsValid())
    {
        FOnlineSessionSettings SessionSettings;

        if(IOnlineSubsystem::Get()->GetSubsystemName() == "NULL") // si queremos testear con el null subsystem hacemos esto true, si queremos conectarnos a través de internet con el steam subsystem sí que ya no nos interesa LAN
        {
            SessionSettings.bIsLANMatch = true;
        }
        else {
            SessionSettings.bIsLANMatch = false;  
        }
        SessionSettings.NumPublicConnections = 5; // si hay este numero de jugadores ya en la sesión, si un jugador quiere buscar esta sesión no le aparecerá en el listado porque ya está llena y no está disponible 
        SessionSettings.bShouldAdvertise = true;  // para que pueda ser encontrada por otros jugadores sin tener que invitarlos
        SessionSettings.bUsesPresence = true;  // para crear un lobby session (So that steam will use lobbies)
        SessionSettings.bUseLobbiesIfAvailable = true;
        SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);   // FName = TEXT("BLABLA")   FString = FString("BLABLA")
        // SessionSettings.Set(TEXT("Test"), FString("Hello"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);    

        SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);  //  nombre de sesión es importante. text macro convierte cualquiera de los distintos tipos de texto que implementa unreal
    }
}

void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
    if (!Success)   // si la sesión es iniciada con el nombre de arriba, cerramos el juego y volvemos a abrirlo sin cerrar la sesión, la sesión (con el nombre de arriba) ya existirá y no nos dejará crearla otra vez. Tenemos que asegurarnos de cerrarla al cerrar el juego porque si no se mantiene forever.
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not create session"));   
        return;
    }

    if (MainMenu != nullptr)
    {
        MainMenu->TearDown();  // esto realmente en vez de llamarlo desde aquí, una mejor alternativa sería que se llame solo sobreescribiendo el método UUserWidget::OnLevelRemovedFromWorld del main menu, el cual se llama cuando vamos a quitar al widget del mundo o del viewport
    }

    UEngine* Engine = GetEngine();
    if (!ensure (Engine != nullptr)) return;
    Engine->AddOnScreenDebugMessage(0, 2, FColor::Green, TEXT("Hosting"));

    UWorld* World = GetWorld();
    if (!ensure (World != nullptr)) return;

    World->ServerTravel("/Game/PuzzlePlatforms/Maps/Lobby?listen");  // el listen del URL es para permitir conexiones de afuera como servidor. Permitir que otros jugadores se unan ya que estamos escuchando solicitudes

}

void UPuzzlePlatformsGameInstance::RefreshServerList()
{
    SessionSearch = MakeShareable(new FOnlineSessionSearch());  // OJO: un new crea algo en el heap, no en el stack. Además, makeShareable convierte un puntero normal cpp a un sharedPTR para unreal (reference counted pointer)
    if(SessionSearch.IsValid())
    {
        // SessionSearch->bIsLanQuery = true;   // en este caso al ser puntero y apuntar al heap podemos usarlo también una vez hayamos encontrado las sesiones para recuperar resultados
        SessionSearch->MaxSearchResults = 100;  // si no solo podremos parsear 1 
        SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);  // allow for search pressence (see other people across the network)
        UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
        SessionInterface->FindSessions(0, SessionSearch.ToSharedRef()); // una TSharedRef no puede ser null, siempre ha de apuntar a algo del heap
    }
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
    if (Success && SessionSearch.IsValid() && MainMenu != nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));
        UE_LOG(LogTemp, Warning, TEXT("Number of sessions: %d"), SessionSearch->SearchResults.Num());
        
        TArray<FServerData> ServerNames;
        // ServerNames.Add("Test Server 1");   // testing purposes
        // ServerNames.Add("Test Server 2");
        // ServerNames.Add("Test Server 3");

        for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)  // RANGED FOR LOOP - también podemos usar auto en vez de poner todo el rollo de FOnlineSession....
        {
            UE_LOG(LogTemp, Warning, TEXT("Found session names: %s"), *SearchResult.GetSessionIdStr());
            FServerData Data;   // este struct lo creamos en MyMainMenu.h
            // Data.Name = SearchResult.GetSessionIdStr();
            Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;    // NumPublicConnections = número de conexiones totales = (disponibles + ocupadas)  por eso las sustraigo de las libres   
            Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;    // conexiones en uso ... (conexiones totales - conexiones libres) = conexiones en uso = número de jugadores   - (OJO: NumOpenPublicConnections siempre es igual a NumPublicConnections en el null online subsystem si no usamos el nombre de sesión default de la constante (bug))  
            Data.HostUsername = SearchResult.Session.OwningUserName;
            
            FString ServerName;
            if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))  // ServerName es un out parameter
            {
                Data.Name = ServerName;
            }
            else
            {
                 Data.Name = "Could not find name";
            }
            ServerNames.Add(Data);
        }
        MainMenu->SetServerList(ServerNames);
    }
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
    if (!SessionInterface.IsValid()) return;
    if (!SessionSearch.IsValid()) return;

    if (MainMenu != nullptr)
    {
        // MainMenu->SetServerList({"Test1", "Test2"});
        MainMenu->TearDown();  // esto realmente en vez de llamarlo desde aquí, una mejor alternativa sería que se llame solo sobreescribiendo el método UUserWidget::OnLevelRemovedFromWorld del main menu, el cual se llama cuando vamos a quitar al widget del mundo o del viewport
    }

    SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);  // podemos chekar el search results porque ya hicimos la búsqueda previamente se supone, si no será null


    // (con la dirección ip directamente antes de implementar el online Subsystem): 

    // UEngine* Engine = GetEngine();
    // if (!ensure (Engine != nullptr)) return;
    // Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));    

    // APlayerController* PlayerController = GetFirstLocalPlayerController();
    // if (!ensure (PlayerController != nullptr)) return;
    // PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);    // nos unimos al server
}


void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!SessionInterface.IsValid()) return;

    FString Address;
    if (!SessionInterface->GetResolvedConnectString(SessionName, Address))  // Address es un out parameter
    {
        UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
        return;
    }

    // Con hamachi podemos conectarnos con gente por internet creando lan over internet. Pero también podemos por el steam subsystem que es lo que vamos a implementar en vez de usar el NULL subsystem de testeo que estábamos usando.
    // para añadir steam subsystem añadimos el plugin Online Subsystem Steam, el módulo "OnlineSubsystemSteam" en el fichero build.cs  y  ir al fichero Config/DefaultEngine.ini  y añadir [OnlineSubsystem] DefaultPlatformService=Steam aparte de más cosas incluidas en la documentación
    // en el fichero Config/DefaultEngine.ini tenemos por defecto el steamDevAppId=480 que es el appId de spacewar la cual es la única gratis  (si no, tenemos que pagar 100 euros para ser developers de steam y recibir otra AppId que podriamos utilizar)
    // el netdriver nos ahorra usar vpn (direct ip connection) ya que nos hace la conexión a través de steam (servicios)

    UEngine* Engine = GetEngine();
    if (!ensure (Engine != nullptr)) return;
    Engine->AddOnScreenDebugMessage(0, 5, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));    

    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure (PlayerController != nullptr)) return;
    PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);    // nos unimos al server (Address es la direccion ip y el puerto)
}


// una vez empezamos la sesión, esta se considerará en progreso y no en espera. Por ello dejará de recibir jugadores y dejará de ser visible también.
void UPuzzlePlatformsGameInstance::StartSession()
{
    if (SessionInterface.IsValid())
    {
        SessionInterface->StartSession(SESSION_NAME); // una vez empezamos la sesión, esta se considerará en progreso y no en espera. Por ello dejará de recibir jugadores y dejará de ser visible también.
    }
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
    APlayerController* PlayerController = GetFirstLocalPlayerController();
    if (!ensure (PlayerController != nullptr)) return;
    PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}




