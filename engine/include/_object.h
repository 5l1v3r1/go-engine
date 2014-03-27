/**
 * @file
 * @brief Estructura de los objeto "GameObject".
 */

#ifndef __OBJECT_H_
#define __OBJECT_H_

#include "_globals.h"
#include "_components.h"
#include "systems/_debug.h"

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
     * El objeto
     *
     * @param name
     */
    CGameObject(std::string name);
    CGameObject();

    virtual ~CGameObject()
    {
      Close();
    }

    void Init();
      virtual void InitCustom(){};
    void Close();
      virtual void CloseCustom(){};

    flags_t Get_flags() {return flags; } // Los flags s�lo deber�an ser maniplados por el objeto, no por el usuario

    bool AddChild(CGameObject* child);
    short int AddChildren(const std::vector<CGameObject*>& children); // Returns 1 on succes. Otherwise, returns the negative index of the last element which failed at AddChild (0, -1, -2, ..., -N)
    //bool AddChildren(const std::initializer_list<CGameObject*>& init) { return }


    bool RemoveChild(std::string name);
    void RemoveChildren();
    CGameObject* GetChild(std::string name);
    CGameObject* GetChild(uint index);
    inline uint GetNumChildren(){return children.size();}

    inline CGameObject* GetParent()
    {
      return Parent;
    }
    inline void SetParent(CGameObject* parent)
    {
      if(parent != NULL)
        Parent = parent;
    }
    inline void RemoveParent()
    {
      Parent->RemoveChild(name);
    }
    inline void UnParent() // no usar nunca desde fuera
    {
      //Parent->RemoveChild(name);
      Parent = NULL;
    }

    inline void SetStartFunction(function_t f)
    {
      start = f;
    }

    inline void SetBehaviourFunction(function_t f)
    {
      behaviour = f;
    }

    inline void SetEventFunction(function_t f)
    {
      event_behaviour = f;
    }

    inline void SetInputFunction(function_t f)
    {
      input_behaviour = f;
    }

    inline void SetRenderFunction(function_t f)
    {
      render = f;
    }

    inline void CallStartFunction()
    {
      if(start) start(this);
    }

    inline void CallBehaviourFunction()
    {
      if(behaviour) behaviour(this);
    }

    inline void CallEventFunction()
    {
      if(event_behaviour) event_behaviour(this);
    }

    inline void CallKeyEventFunction()
    {
      if(input_behaviour) input_behaviour(this);
    }

    inline void CallRenderFunction()
    {
      if(render) render(this);
    }

  public:
    int AddToSystem();
    bool RemoveFromSystem();

    void OnEvent();
    void OnInput();
    void OnLoop();
    void OnRender(glm::mat4 projMatrix, glm::mat4 modelViewMatrix);
    //void OnRenderDebug();

  protected:
    void Register(uint ID)
    {
      id = ID;
    }

  public:

    inline std::string GetName()
    {
      return name;
    }
    inline int GetID()
    {
      return id;
    }

    void Enable(bool recursive = false);
    void Disable(bool recursive = false);
    void SetState(bool state, bool recursive = false);

    void Preserve(bool recursive = false);
    void UnPreserve(bool recursive = false);
    void SetPreserve(bool state, bool recursive = false);

    inline bool IsEnabled()
    {
      return enabled;
    }

    inline bool IsInited()
    {
      return inited;
    }

    inline bool IsPreserved()
    {
      return preserve;
    }

    bool NearBy(CGameObject* go, double distance);

    /** PLANTILLAS **/
    template <class Type>
    Type* GetComponent();

    CComponent* GetComponent(Components::components_t c)
    {
      if(components.find(c) != components.end())
        return components[c];

      return NULL;
    }

    template <class Type>
    bool AddComponent();

    template <class Type>
    bool RemoveComponent();

    template <class Type>
    void SetComponentStateComponent(bool state);
	
    template <class Type>
    void SetData(input_t data);

    template <class Type>
    output_t GetData();
    /**
     * Funciones para acceso directo a los componentes aqu�:
     */

    /**
     * CComponent_Transform* transform() {return (CComponent_Transform*) components[components::transform]; }
     *
     * Y as� para los componentes principales.
     * Si no existe un componente, se crea. A diferencia de GetComponent<...>(), si el componente no existe, devuelve NULL.
     * **/

    inline CComponent_Transform* Transform()
    {
      // Siempre existe un componente Transform
      return (CComponent_Transform*)components[Components::transform];
    }

    inline CComponent_Camera* Camera()
    {
      if(components.find(Components::camera) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::camera, new CComponent_Camera(this)));

      return (CComponent_Camera*)components[Components::camera];
    }

    inline CComponent_Mesh_Render* MeshRender()
    {
      if(components.find(Components::mesh_render) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::mesh_render, new CComponent_Mesh_Render(this)));

      return (CComponent_Mesh_Render*)components[Components::mesh_render];
    }

    inline CComponent_Particle_Emitter* ParticleEmitter()
    {
      if(components.find(Components::particle_emitter) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::particle_emitter, new CComponent_Particle_Emitter(this)));

      return (CComponent_Particle_Emitter*)components[Components::particle_emitter];
    }

    inline CComponent_GUI_Texture* GUITexture()
    {
      if(components.find(Components::gui_texture) == components.end())
        components.insert(std::pair<int, CComponent*>(Components::gui_texture, new CComponent_GUI_Texture(this)));

      return (CComponent_GUI_Texture*)components[Components::gui_texture];
    }

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

#endif /* __OBJECT_H_ */
