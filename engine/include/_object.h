/**
 * @file
 * @brief Estructura de los objeto "GameObject".
 */

#ifndef __OBJECT_H_
#define __OBJECT_H_

#include "_globals.h"
#include "_components.h"
#include "systems/_debug.h"

//namespace GO {

// In�til (de momento)
/**
 * Enum sin uso.
 */
enum gameObject_flags { gof_none = 0x00, gof_render = 0x01, gof_event = 0x02, gof_kevent = 0x04, gof_loop = 0x08 };
/**
 * Enum sin uso.
 */
enum gameObject_type {gameObject_empty = 0};//!< gameObject_empty

/**
 * Puntero a funci�n miembro de un game object.
 *
 * El puntero se usar� para funciones del tipo "void f(CGameObject* g)". V�ase CGameObject::start,
 * CGameObject:: behaviour, CGameObject:: event_behaviour, CGameObject:: input_behaviour, CGameObject:: render.
 *
 * Se usar� para guardar en el game object una serie de punteros a funciones externas para ser llamadas en determinados momentos,
 * usando como referencia al objeto el par�metro "self".
 *
 * @param self Referencia a un objeto dado.
 */
typedef void (*function_t)(CGameObject* self);

// Nota: a�adir "CGameObject_NULL" que no haga nada en sus operaciones. As�, si el manager devuelve un NULL, y se trata de acceder a un m�todo de ese NULL, no se har� nada
// -> Ya que devolver� CGameObject_NULL, y no NULL
// -> Si no, crear una instancia de CGameObject llamado "GAMEOBJECT_NULL", o algo por el estilo
// -> O un flag tipo "null_object" en los objetos actuales que no haga nada en las funciones si el objeto es nulo.

/**
 * Clase que representa a un objeto de juego.
 *
 * Para entender mejor esta parte que voy a explicar, recomiendo leer el art�culo siguiente: http://www.genbetadev.com/programacion-de-videojuegos/diseno-de-videojuegos-orientado-a-entidades-y-componentes
 *
 * @image html diagrama1.png
 *
 * La unidad b�sica con la que trabajar� nuestro motor gr�fico de cara al usuario que desarrollar� el juego ser� los objectos que realizar�n una abstracci�n de cualquier cosa, a los que hemos llamado cari�osamente �Game Objects�. Un Game Object, por consiguiente, es la representaci�n de una entidad en el mundo real. Cada objeto debe tener un nombre para referirnos a �l ("coche1", "coche1_rueda1", etc.) y un identificador (que ser� usado de cara al sistema). Por ejemplo, un coche ser�a uno, y las ruedas que lo componen, tambi�n.
 *
 * Ahora bien, si el coche se mueve, se deben mover las ruedas, las luces, los ejes� por lo que diremos que esos objetos est�n emparentados (apartado 1 en la imagen adjunta). Creando esta jerarqu�a, conseguimos agrupar de forma l�gica los objetos, d�ndose el caso de que los objetos puedan tener varios hijos (o children) y un �nico o ning�n padre, creando una relaci�n bidireccional entre el padre y sus hijos por medio de una lista de punteros y un �nico puntero, respectivamente. Evidentemente, no es posible que un padre sea padre de s� mismo, directa o indirectamente.
 *
 * De todas formas, no todos los objetos son iguales, �verdad?. Algunos est�n definidos con una serie de valores y propiedades distintas a los dem�s. Por ejemplo, una pelota tiene un modelo de una pelota 3D (una esfera), un material (pl�stico), una esfera de colisi�n� mientras que un temporizador s�lo posee una parte l�gica que determina si ha transcurrido un lapso de tiempo. �C�mo distinguimos pues unos objetos de otros? Lo haremos mediante componentes. Cada objeto puede tener, a lo sumo, un componente de cada tipo. Por ejemplo, podemos definir varios componentes:
 * <ul>
 * <li><b>Transformaci�n:</b> Posici�n, rotaci�n y escala de un objeto en el mundo 3D.
 * <li><b>C�mara:</b> Define una c�mara desde la cual se har� un renderizado de una escena.
 * <li><b>Texto de interfaz gr�fica de usuario</b> (GUI Text).
 * <li><b>Luz est�tica:</b> Usada para iluminar el escenario�
 * </ul>
 * Y as�, se puede crear una lista muy larga de componentes que nos servir� para personalizar nuestros componentes. En el apartado 2 se pueden ver algunas formas de invocar funciones desde el objeto para obtener o modificar datos de los componentes. Intentar� dedicar un post a ello.
 *
 * Por si fuera poco, vamos a definir 4 nuevos punteros para personalizar los objetos, que ser�n punteros a funciones que nos permitir�n hacer llamadas a funciones dependiendo de su tipo:
 * <ul>
 * <li><b>start:</b> Se llama al inicio, cuando se crea el objeto y se a�ade al sistema.
 * <li><b>behaviour:</b> Se llama en cada iteraci�n, y determina el pr�ximo estado del objeto.
 * <li><b>event:</b> Se llama cada vez que se ejecute un evento (movimiento del rat�n, reescalado de ventana, etc.).
 * <li><b>input:</b> Se llama cada vez que se produce una entrada de teclado, joystick, rat�n, etc. Usar antes que "event".
 * <li><b>rener:</b> Se llama cada vez que se va a redenderizar  una tecla del teclado.
 * </ul>
 * Por ejemplo, se define una funci�n �behaviour� para un objeto de la siguiente forma:
 *
 @code
  void GO1_movimiento(CGameObject* gameObject)
  {
    // Operaciones para el objeto que vaya a usar esta funci�n ir�n aqu�.
  }

  CGameObject* GO1 = gSystem_GameObject_Manager.AddGameObject("GO1"); // Crear objeto
  GO1->SetKeyEventBehaviourFunction(&GO1_movimiento);                 // Cargar funci�n behaviour
@endcode
 * Que ser� llamada en cada iteraci�n de la ejecuci�n de nuestra aplicaci�n, pasando como par�metro el puntero del objeto que apunta a s� mismo (this, b�sicamente). El objeto llamado gSystem_GameObject_Manager ser� el sistema que se encargar� de gestionar nuestros objetos, del que me encargar� de explicar en el pr�ximo post.
 */
class CGameObject
{
  friend class CComponent;
  friend class CComponent_Transform;
  friend class CSystem_GameObject_Manager;
  friend class CSystem_Debug;
  friend class CSystem_Render;

  protected:
    flags_t flags;
    bool inited;
    bool enabled;
    bool preserve;
    //bool DontDeleteOnLoad y void DontDeleteOnLoad();

    std::string name;
    int id;

    std::map<int, CComponent*> components;
    std::map<std::string, CGameObject*> children;

    CGameObject* Parent;

    function_t start;
    function_t behaviour;
    function_t event_behaviour;
    function_t input_behaviour;
    function_t render;

  private:
    /*template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & flags;
      ar & inited;
      ar & enabled;
      ar & name;
      ar & id;

      ar & components; // fallo al guardar objetos instanciados por punteros (boost::serialization)
      ar & children;

      // Inservible
      //ar & start & behaviour & event_behaviour & keyevent_behaviour;
    }*/

  public:
    /**
     * Constructor principal.
     *
     * Construye un objeto "vac�o" dado un nombre. Crear un objeto no implica que se a�ada al sistema gestor de objetos CSystem_GameObject_Manager.
     *
     * El objeto poseer� un nombre especificado por "name" y tendr� un componente inicial, CComponent_Transform, que indicar� su siutaci�n, orientaci�n y tama�o en el espacio.
     *
     * @param name Nombre del game object.
     */
    CGameObject(std::string name);

    /**
     * Constructor vac�o.
     *
     * Crea un objeto vac�o, sin propiedades y sin usar atributos. Usado principalmente para copias y pruebas.
     */
    CGameObject();

    /**
     * Destructor principal.
     *
     * Desactiva por completo el objeto, adem�s de eliminar todos sus componentes y atributos.
     */
    virtual ~CGameObject()
    {
      Close();
    }

    /**
     * Iniciar el objeto.
     *
     * Inicializa el gameObject, haciendo que pase a estar activado, adem�s de a�adir el componente CComponent_Transform (por si no ha sido a�adido, claro).
     * Adem�s, el objeto queda marcado como <b>iniciado</b> y como <b>activo.</b> S�lo podr� ser iniciado si no ha sido iniciado previamente, osi ha sido cerrado.
     */
    void Init();
      /** <span style="color: red; font-weight: bold;">Sin usar</span>. */
      virtual void InitCustom(){};
    /**
     * Cerrar el objeto.
     *
     * Una vez iniciado, el gameObject puede ser cerrado, eliminando todos sus <b>componentes</b> e <b>hijos</b>.  Adem�s, el objeto es marcado como
     * <b>no iniciado</b> y como <b>desactivado</b>.
     */
    void Close();
      /** <span style="color: red; font-weight: bold;">Sin usar</span>. */
      virtual void CloseCustom(){};

    /** <span style="color: red; font-weight: bold;">Sin usar</span>. */
    flags_t Get_flags() {return flags; } // Los flags s�lo deber�an ser maniplados por el objeto, no por el usuario

    /**
     * A�adir un nuevo hijo.
     *
     * A�ade un nuevo hijo al objeto actual dado otro objeto existente, siempre y cuando sea posible.
     * S�lo se podr� a�adir un hijo si el padre no es hijo de alguno de los hijos del objeto a a�adir, o si el onjeto no es id�ntico al objeto a a�adir.
     * Esto significa que no puede haber ciclos en la jerarqu�a de gameObjects. Finalmente, asigna al hijo a�adido, si todo ha funcionado correctamente, un padre tal que sea el objeto actual.
     *
     * @warning En caso de a�adir un objeto con un nombre ya existente, se perder� la referencia al objeto a�adido anteriormente. Aseg�rese primero de que el objeto no es hijo del padre antes de a�adirlo.
     *
     * @param child Puntero a gameObject a a�adir. Debe estar inicializado correctamente.
     * @return true si a�ade el hijo correctamente, false en cualquier otro caso.
     */
    bool AddChild(CGameObject* child);
    /**
     * A�adir una lista de hijos.
     *
     * Dada una lista de hijos, se pueden a�adir al objeto actual todos los hijos de la lista.
     * Se intentar� a�adir hijo a hijo todos los posibles que esten en la lista, siempre y cuando cumplan las condiciones de CGameObject::AddChild().
     * A todo objeto a�adido, se le asignar� como padre el objeto actual.
     *
     * Por ejemplo, se podr� usar este m�todo de la siguiente manera:
     *
     @code
     CGameObject* go1  = new CGameObject("go1");
     CGameObject* go11 = new CGameObject("go11");
     CGameObject* go12 = new CGameObject("go12");

     go1->AddChildren({go11, go12});
     @endcode
     * @bug Para listas de objetos de tama�o menor que 2, es posible que haya que especificar el tipo, tal que se represente como <span><i>std::vector<CGameObject*>{go11, go12}</i></span>
     *
     * @param children Array que contendr� los punteros a los hijos a a�adir.
     * @return En caso de �xtio, devuelve el valor "1". En caso de fallo, devuelve el �ndice de la �ltima posici�n en la que fall�, con valor negativo (0, -1, -2, ..., -(n-1)).
     */
    short int AddChildren(const std::vector<CGameObject*>& children); // Returns 1 on succes. Otherwise, returns the negative index of the last element which failed at AddChild (0, -1, -2, ..., -N)
    //bool AddChildren(const std::initializer_list<CGameObject*>& init) { return }

    /**
     * Eliminar un hijo.
     *
     * Elimina un hijo del objeto de juego.
     * @warning El hijo ser� eliminado de la lista de hijos, pero no ser� borrado del sistema. Esto es, no se llamar� a CGameObject::Close() ni se har� un **delete**.
     * @param name Nombre del hijo a eliminar.
     * @return En caso de borrarlo correctamente, devuelve true. En otro caso, retorna false.
     */
    bool RemoveChild(std::string name);
    /**
     * Eliminar a todos los hijos.
     *
     * Elimina a todos los hijos del objeto actual.
     * @warning Los hijos ser�m eliminados de la lista de hijos, pero no ser�m borrados del sistema. Esto es, no se llamar� a CGameObject::Close() ni se har�n **delete**.
     */
    void RemoveChildren();

    /**
     * Obtener un puntero a un hijo.
     *
     * Dado un nombre de un hijo, retorna un puntero a la estructura de datos que representa a ese hijo.
     * @param name Nombre del objeto a obtener.
     * @return Devuelve un puntero al objeto si es un hijo del objeto actual. En otro caso, devuelve NULL.
     */
    CGameObject* GetChild(std::string name);
    /**
     * Obtiene un puntero a un hijo.
     *
     * Dado un �ndice, se acceder� a un hijo que ocupe dicho �ndice. Los hijos ser�n ordenados por orden alfab�tico (funcionamiento de un std::map).
     * @param index Valor entre 0 y el n�mero de hijos actual. V�ase CGameObject::GetNumChildren().
     * @return Devuelve el puntero al objeto si es un hijo del objeto actual. En otro caso, devuelve NULL.
     */
    CGameObject* GetChild(uint index);
    /**
     * N�mero de hijos.
     *
     * Retorna el n�mero de hijos del objeto actual.
     * @return N�mero de hijos del objeto actual (tama�o de la lista de hijos).
     */
    inline uint GetNumChildren(){return children.size();}

    /**
     * Obtener el padre.
     *
     * @return Retorna un puntero al padre que lleva a este hijo. Si es hu�rfano, devolver� NULL.
     */
    inline CGameObject* GetParent()
    {
      return Parent;
    }

    /**
     * Elimina el padre.
     *
     * Si el objeto tiene un padre, tratar� de volverse hu�rfano, eliminando el puntero a su padre, y limpiando la referencia del padre al hijo en el padre.
     */
    inline void RemoveParent()
    {
      Parent->RemoveChild(name);
    }

    /**
     * Cambiar el callback "start".
     *
     * Cambia el puntero del callback **Start**, que ser� una funci�n que se llamar� al iniciar el objeto.
     * @param f Puntero a funci�n externa. Si es NULL, suponemos que no se usar� ning�n callback.
     */
    inline void SetStartFunction(function_t f)
    {
      start = f;
    }

    /**
     * Cambiar el callback "behaviour".
     *
     * Cambia el puntero del callback **Behaviour**, que ser� una funci�n que se llamar� en cada iteraci�n del juego (actualizaci�n constante).
     * @param f Puntero a funci�n externa. Si es NULL, suponemos que no se usar� ning�n callback.
     */
    inline void SetBehaviourFunction(function_t f)
    {
      behaviour = f;
    }

    /**
     * Cambiar el callback "event".
     *
     * Cambia el puntero del callback **Event**, que ser� una funci�n que se llamar� cada vez que se produzca un evento.
     * @param f Puntero a funci�n externa. Si es NULL, suponemos que no se usar� ning�n callback.
     */
    inline void SetEventFunction(function_t f)
    {
      event_behaviour = f;
    }

    /**
     * Cambiar el callback "input".
     *
     * Cambia el puntero del callback **Event**, que ser� una funci�n que se llamar� en cada iteraci�n del juego, que procesar� datos de entrada (teclado, ret�n, joystick...). V�ase CSystem_UserInput.
     * @param f Puntero a funci�n externa. Si es NULL, suponemos que no se usar� ning�n callback.
     */
    inline void SetInputFunction(function_t f)
    {
      input_behaviour = f;
    }

    /**
     * Cambiar el callback "render".
     *
     * Cambia el puntero del callback **Render**, que ser� una funci�n que se llamar� en cada iteraci�n del juego, a la hora de dibujar el objeto por pantalla.
     * @param f Puntero a funci�n externa. Si es NULL, suponemos que no se usar� ning�n callback.
     */
    inline void SetRenderFunction(function_t f)
    {
      render = f;
    }

    /**
     * LLama al callback **Start**, siempre y cuando no sea NULL. Ser� llamado de manera autom�tica desde CGameObject.Init().
     */
    inline void CallStartFunction()
    {
      if(start) start(this);
    }

    /**
     * LLama al callback **Behaviour**, siempre y cuando no sea NULL. Ser� llamado de manera autom�tica desde CGameObject.OnLoop().
     */
    inline void CallBehaviourFunction()
    {
      if(behaviour) behaviour(this);
    }


    /**
     * LLama al callback *Event**, siempre y cuando no sea NULL. Ser� llamado de manera autom�tica desde CGameObject.OnEvent().
     */
    inline void CallEventFunction()
    {
      if(event_behaviour) event_behaviour(this);
    }


    /**
     * LLama al callback **Input**, siempre y cuando no sea NULL. Ser� llamado de manera autom�tica desde CGameObject.OnInput().
     */
    inline void CallInputFunction()
    {
      if(input_behaviour) input_behaviour(this);
    }

    /**
     * LLama al callback **Render**, siempre y cuando no sea NULL. Ser� llamado de manera autom�tica desde CGameObject.OnRender().
     */
    inline void CallRenderFunction()
    {
      if(render) render(this);
    }

  public:
    /**
     * Registrar un objeto en el sistema.
     *
     * Registra un objeto de juego en la lista de objetos globales, administrado por el sistema CSystem_GameObject_Manager.
     * Un objeto no podr� ser a�adido si ya existe un objeto con su nombre, o si su identificador es inv�lido (v�ase Utils::validateIdentifier).
     * @return Si se registra correctamente, devuelve un identificador num�rico del objeto. En caso contrario, devuelve -1.
     */
    int AddToSystem();
    /**
     * Elimina un objeto del sistema.
     *
     * Elimina un objeto de la lista de objetos globales, administrado por el sistema CSystem_GameObject_Manager.
     * Un objeto no podr� ser eliminado si no est� registrado.
     * @return Devuelve true si se ha eliminado del sistema correctamente, false en cualquier otro caso.
     */
    bool RemoveFromSystem();

    /**
     * Gestor de eventos.
     *
     * Funci�n para gestionar los eventos del objeto y de todos sus componentes. Ser� llamado desde CSystem_GameObject_Manager::OnEvent(), al principio de cada iteraci�n.
     *
     * Si el objeto no est� en modo **activo** e **iniciado**, no se llamar� a esta funci�n.
     */
    void OnEvent();
    /**
     * Gestor de entrada.
     *
     * Funci�n para gestionar la entrada (rat�n, teclado, etc.) y de todos sus componentes. Ser� llamado desde CSystem_GameObject_Manager::OnInput(), despu�s de CSystem_GameObject_Manager::OnEvent().
     *
     * Si el objeto no est� en modo **activo** e **iniciado**, no se llamar� a esta funci�n.
     */
    void OnInput();
    /**
     * Gestor de comportamientos.
     *
     * Funci�n para gestionar el comportamiento del objeto y de todos sus componentesen cada iteraci�n. Ser� llamado desde CSystem_GameObject_Manager::OnLoop(), despu�s de CSystem_GameObject_Manager::OnInput().
     *
     * Si el objeto no est� en modo **activo** e **iniciado**, no se llamar� a esta funci�n.
     */
    void OnLoop();
    /**
     * Gestor de renderizado.
     *
     * Funci�n para gestionar el dibujado del objeto y de todos sus componentes. Ser� llamado desde CSystem_GameObject_Manager::OnRender(), despu�s de CSystem_GameObject_Manager::OnLoop().
     *
     * Si el objeto no est� en modo **activo** e **iniciado**, no se llamar� a esta funci�n.
     */
    void OnRender(glm::mat4 projMatrix, glm::mat4 modelViewMatrix);
    //void OnRenderDebug();

  protected:
    void Register(uint ID)
    {
      id = ID;
    }

    inline void SetParent(CGameObject* parent)
    {
      if(parent != NULL)
        Parent = parent;
    }

    inline void UnParent() // no usar nunca desde fuera
    {
      //Parent->RemoveChild(name);
      Parent = NULL;
    }

  public:

    /**
     * Getter del nombre del objeto.
     * @return Nombre del objeto. Si est� registrado en el sistema, se asegura de que es un identificador v�lido.
     */
    inline std::string GetName()
    {
      return name;
    }
    /**
     * Getter del identificador del objeto en el sistema.
     * @return Si est� registrado, el identificador tiene un valor mayor o igual que 0. En otro caso, vale -1.
     */
    inline int GetID()
    {
      return id;
    }

    /**
     * Activa el objeto.
     *
     * Cambia el estado del objeto actual por **activo**.
     * @param recursive Si es true, se activar�n todos sus hijos de manera recursiva. En caso contrario, s�lo se activar� el objeto actual.
     */
    void Enable(bool recursive = false);
    /**
     * Desactiva el objeto.
     *
     * Cambia el estado del objeto actual por **desactivado**.
     * @param recursive Si es true, se desactivar�n todos sus hijos de manera recursiva. En caso contrario, s�lo se desactivar� el objeto actual.
     */
    void Disable(bool recursive = false);
    /**
     * Cambiar el estado del objeto.
     *
     * Cambia el estado del objeto actual por el estado especificado por "state" (true -> **activo**, false -> **desactivado**.
     *
     * @param state Nuevo estado de los objetos.
     * @param recursive Si es true, se cambiar� el estado todos sus hijos de manera recursiva. En caso contrario, s�lo se cambiar� el objeto actual.
     */
    void SetState(bool state, bool recursive = false);

    /**
     * Preservar el objeto entre estancias.
     *
     * Un objeto se conoce como que est� preservado si mantiene sus datos intactos entre los cambios de estancia. B�sicamente, son objetos que
     * no ser� borrados al cambiar de estancia o nivel, y que permanecer� con el mismo nombre y el mismo contenido que en su �ltima estancia.
     *
     * V�ase CEngine y CInstance.
     * @param recursive  Si es true, se marcar�n como ***preservar*** todos sus hijos de manera recursiva. En caso contrario, s�lo se cambiar� el objeto actual.
     */
    void Preserve(bool recursive = false);
    /**
     * No preservar el objeto entre estancias.
     *
     * Ant�nimo de CGameObject::Preserve().
     *
     * V�ase CEngine y CInstance.
     * @param recursive  Si es true, se marcar�n como ***no preservar*** todos sus hijos de manera recursiva. En caso contrario, s�lo se cambiar� el objeto actual.
     */
    void UnPreserve(bool recursive = false);
    /**
     * Cambiar preservaci�n del objeto entre estancias.
     *
     * Combina CGameObject::Preserve() y CGameObject::UnPreserve().
     *
     * V�ase CEngine y CInstance.
     *
     * @param state Nuevo estado (true -> **preservar**, false -> **no preservar**)
     * @param recursive  Si es true, se marcar�n como el estado indicado a todos sus hijos de manera recursiva. En caso contrario, s�lo se cambiar� el objeto actual.
     */
    void SetPreserve(bool state, bool recursive = false);

    /**
     * Preguntar si el objeto est� **activado**.
     *
     * @return Retorna true si el objeto est� **activado**. false en caso contrario.
     */
    inline bool IsEnabled()
    {
      return enabled;
    }
    /**
     * Preguntar si el objeto est� **iniciado**.
     *
     * @return Retorna true si el objeto est� **iniciado**. false en caso contrario.
     */
    inline bool IsInited()
    {
      return inited;
    }

    /**
     * Preguntar si el objeto est� marcado como **preservado**.
     *
     * @return Retorna true si el objeto est� marcado como **preservado**. false en caso contrario.
     */
    inline bool IsPreserved()
    {
      return preserve;
    }

    /**
     * Comprobador de cercan�a.
     *
     * Dado otro objeto, el objeto actual y una distancia, se comprobar� si la distancia entre el origen de ambos objetos es menor a la distancia propuesta. Se usar� la funci�n de distancia eucl�dea.
     * @param go Otro objeto.
     * @param distance Distancia entre ambos objetos.
     * @return Devuelve true si la distancia eucl�dea entre los 2 objetos es menor que "distance". false en caso contrario.
     */
    bool NearBy(CGameObject* go, double distance);

    /* PLANTILLAS */
    /**
     * Obtener componente.
     *
     * Dada la funci�n plantilla actual, se podr� obtener un componente seleccionado, siempre y cuando exista.
     *
     * Por ejemplo, podemos usar:
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->GetComponent<CComponent_Transform*>()->position.x = 0; // Acceder al componente "transform" y cambiar un atributo.
     @endcode
     *
     * V�ase CComponent.
     * @return Devuelve un puntero del tipo del componente solicitado. NULL si no existe el componente.
     */
    template <class Type>
    Type* GetComponent();

    /**
     * Obtener componente (gen�rico).
     *
     * Obtiene un componente del tipo "CComponent*", siempre y cuando exista el identificador del componente asociado.
     *
     * V�ase Components::components_t y CComponent.
     * @param c Tipo de componente (enum).
     * @return Puntero al componente, si existe. NULL en otro caso.
     */
    CComponent* GetComponent(Components::components_t c)
    {
      if(components.find(c) != components.end())
        return components[c];

      return NULL;
    }

    /**
     * A�adir un componente.
     *
     * Dada la funci�n plantilla actual, se podr� a�adir un componente del tipo indicado, siempre y cuando exista.
     *
     * @warning Solo puede haber un componente de cada tipo.
     *
     * Por ejemplo, podemos usar:
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->AddComponent<CComponent_Camera*>(); // A�ade el componente "camera".
     @endcode
     *
     * V�ase CComponent.
     * @return Retorna true si se ha a�adido el componente, false en caso contrario.
     */
    template <class Type>
    bool AddComponent();

    /**
     * Eliminar un componente.
     *
     * Dada la funci�n plantilla actual, se podr� eliminar un componente seleccionado, siempre y cuando exista en el objeto. El componente ser� destruido por completo.
     *
     * @warning No se puede eliminar el componente CComponent_Transform.
     *
     * Por ejemplo, podemos usar:
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->RemoveComponent<CComponent_Camera*>(); // Eliminar el componente "camera".
     @endcode
     *
     * V�ase CComponent.
     * @return Retorna true si se ha borrado correctamente, false en caso contrario.
     */
    template <class Type>
    bool RemoveComponent();

    /**
     * Cambiar el estado de un componente.
     *
     * Dada la funci�n plantilla actual, se podr� cambiar el estado de un componente seleccionado, siempre y cuando exista, por **activado** o **desactivado**.
     *
     * Por ejemplo, podemos usar:
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->SetComponentState<CComponent_Camera*>(true); // Activar el componente c�mara.
     @endcode
     *
     * V�ase CComponent.
     * @return Devuelve un puntero del tipo del componente solicitado. NULL si no existe el componente.
     */
    template <class Type>
    void SetComponentStateComponent(bool state);
	
    /** <span style="color: red; font-weight: bold;">Sin usar</span>. **/
    template <class Type>
    void SetData(input_t data);

    /** <span style="color: red; font-weight: bold;">Sin usar</span>. **/
    template <class Type>
    output_t GetData();
    /* Funciones para acceso directo a los componentes aqu�: */

    /* CComponent_Transform* transform() {return (CComponent_Transform*) components[components::transform]; }
     *
     * Y as� para los componentes principales.
     * Si no existe un componente, se crea. A diferencia de GetComponent<...>(), si el componente no existe, devuelve NULL.
     */

    /**
     * Acceso directo para el componente **transform** (CComponent_Transform).
     *
     * Accede de manera sencilla al componente **transform**.
     *
     * Por ejemplo, usaremos:
     *
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->Transform()->position.x; // Acceder a un atributo del componente.
     // go1->GetComponent<CComponent_Transform*>()->position.x;
     @endcode
     *
     * V�ase CComponent_Transform.
     *
     * @return Puntero al componente **transform**.
     */
    inline CComponent_Transform* Transform()
    {
      // Siempre existe un componente Transform
      return (CComponent_Transform*)components[Components::transform];
    }

    /**
     * Acceso directo para el componente **camera** (CComponent_Camera).
     *
     * Accede de manera sencilla al componente **camera**.
     *
     * Por ejemplo, usaremos:
     *
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->Camera()->field_of_view; // Acceder a un atributo del componente.
     @endcode
     *
     * Si el componente no existe, ser� creado, y luego ser� retornado.
     *
     * V�ase CComponent_Camera.
     *
     * @return Puntero al componente **camera**.
     */
    inline CComponent_Camera* Camera()
    {
      if(components.find(Components::camera) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::camera, new CComponent_Camera(this)));

      return (CComponent_Camera*)components[Components::camera];
    }

    /**
     * Acceso directo para el componente **mesh render** (CComponent_Mesh_render).
     *
     * Accede de manera sencilla al componente **mesh render**.
     *
     * Por ejemplo, usaremos:
     *
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->Camera()->field_of_view; // Acceder a un atributo del componente.
     @endcode
     *
     * Si el componente no existe, ser� creado, y luego ser� retornado.
     *
     * V�ase CComponent_Mesh_Render.
     *
     * @return Puntero al componente **mesh render**.
     */
    inline CComponent_Mesh_Render* MeshRender()
    {
      if(components.find(Components::mesh_render) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::mesh_render, new CComponent_Mesh_Render(this)));

      return (CComponent_Mesh_Render*)components[Components::mesh_render];
    }

    /**
     * Acceso directo para el componente particle emitter (CComponent_Particle_Emitter).
     *
     * Accede de manera sencilla al componente **particle emitter**.
     *
     * Por ejemplo, usaremos:
     *
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->ParticleEmitter()->color; // Acceder a un atributo del componente.
     @endcode
     *
     * Si el componente no existe, ser� creado, y luego ser� retornado.
     *
     * V�ase CComponent_Particle_Emitter.
     *
     * @return Puntero al componente **particle emitter**.
     */
    inline CComponent_Particle_Emitter* ParticleEmitter()
    {
      if(components.find(Components::particle_emitter) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::particle_emitter, new CComponent_Particle_Emitter(this)));

      return (CComponent_Particle_Emitter*)components[Components::particle_emitter];
    }

    /**
     * Acceso directo para el componente **gui texture** (CComponent_GUI_Texture).
     *
     * Accede de manera sencilla al componente **gui texture**.
     *
     * Por ejemplo, usaremos:
     *
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->GUITexture()->width; // Acceder a un atributo del componente.
     @endcode
     *
     * Si el componente no existe, ser� creado, y luego ser� retornado.
     *
     * V�ase CComponent_GUI_Texture.
     *
     * @return Puntero al componente **gui texture**.
     */
    inline CComponent_GUI_Texture* GUITexture()
    {
      if(components.find(Components::gui_texture) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::gui_texture, new CComponent_GUI_Texture(this)));

      return (CComponent_GUI_Texture*)components[Components::gui_texture];
    }

    /**
     * Acceso directo para el componente **audio source** (CComponent_Audio_Source).
     *
     * Accede de manera sencilla al componente **audio source**.
     *
     * Por ejemplo, usaremos:
     *
     @code
     CGameObject go1 = new CGameObject("go1");
     go1->AudioSource()->volume; // Acceder a un atributo del componente.
     @endcode
     *
     * Si el componente no existe, ser� creado, y luego ser� retornado.
     *
     * V�ase CComponent_Audio_Source.
     *
     * @return Puntero al componente **gui texture**.
     */
    inline CComponent_Audio_Source* AudioSource()
    {
      if(components.find(Components::audio_source) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::audio_source, new CComponent_Audio_Source(this)));

      return (CComponent_Audio_Source*)components[Components::audio_source];
    }
};

template <class Type>
Type* CGameObject::GetComponent()
{
  if(components.find(Type::GetID()) != components.end())
  {
    return (Type*)components[Type::GetID()];
  }
  //gSystem_Debug.console_error_msg("From CGameObject \"%s\": Could not find component \"%s\"", name.c_str(), components::component_to_string((components::components)Type::GetID()));
  return NULL;
}

template <class Type>
bool CGameObject::AddComponent()
{
  int id = Type::GetID();
  if(components.find(id) != components.end())
    return false;

  components.insert(std::pair<int, CComponent*>(id, new Type(this)));
  return true;
}

template <class Type>
bool CGameObject::RemoveComponent()
{
  int id = Type::GetID();
  if(id == Components::transform)
	  return false;
	
  std::map<int, CComponent*>::iterator it = components.find(id);
  if(it != components.end())
  {
    delete it->second;
    components.erase(it);

    return true;
  }

  return false;
}

template <class Type>
void CGameObject::SetComponentStateComponent(bool state)
{
  if(components.find(Type::GetID()) != components.end())
    components[Type::GetID()]->SetState(state);
}

template <class Type>
void CGameObject::SetData(input_t data)
{
  int id = Type::GetID();
  if(components.find(id) != components.end() && data)
    components[id]->Set(data);
}

template <class Type>
output_t CGameObject::GetData()
{
  int id = Type::GetID();
  if(components.find(id) != components.end())
    return components[id]->Get();
  else
    return NULL;
}

//}

#endif /* __OBJECT_H_ */

//using namespace GO;
