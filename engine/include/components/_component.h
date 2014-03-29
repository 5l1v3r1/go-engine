/**
 * @file
 * @brief Fichero que incluye la clase abstracta CComponent para definir componentes.
 */

#ifndef __COMPONENT_H_
#define __COMPONENT_H_

#include "_globals.h"

/** @addtogroup Componentes */
/*@{*/

/**
 * @brief Componentes.
 *
 * Espacio de nombres para guardar enumeraciones, variables y m�todos referentes a los componentes.
 *
 * @warning Esto deber�a en otro sitio m�s generico, no en el fichero de definici�n del componente abstracto.
 */
namespace Components // ->Nota El namespace Components deber�a ir en un lugar m�s gen�rico.
{
  /**
   * @brief Enumeraci�n o identificadores de componentes.
   *
   * Enum para identificar a todos los tipos de componentes. Cada clase heredada de CComponent debe tener un valor en esta enumeraci�n.
   */
  enum components_t { base = 0, camera, mesh_render, particle_emitter, gui_texture, audio_source, transform, dummy1, dummy2, dummy3, __component_not_defined};

  /**
   * @brief Nombres de componentes.
   *
   * Array de strings que contienen, por cada enum de components_t, una representaci�n con caracteres.
   * La representaci�n como cadena de caracteres ser� id�ntica al nombre del valor del enum, salvo para el caso especial components_t.__component_not_defined, que ser� *"not_defined"*.
   */
  extern const char* components_s[];

  /**
   * @brief Transformar un componente en una string.
   *
   * Dado un enum de un componente, transforma dicho valor a un string.
   * @param c Valor del enum.
   * @return Representaci�n textual del componente.
   */
  const char* component_to_string(components_t c);

  /**
   * @brief Transformar un string a un enum de un componente.
   *
   * Dado un nombre, trata de transformar dicho nombre a una enum de componentes.
   * @param c Nombre del componente.
   * @return En caso de encontrar un componente, devuelve su valor de enum. En caso contrario, devuelve components_t.__component_not_defined.
   */
  components_t string_to_component(const std::string& c);
}

class CGameObject;

/**
 * @brief Puntero a funci�n miembro de un game object.
 *
 * El puntero se usar� para funciones del tipo "void f(CGameObject* g)".
 *
 * @see CGameObject::start
 * @see CGameObject::behaviour
 * @see CGameObject::event_behaviour
 * @see CGameObject::input_behaviour
 * @see CGameObject::render
 *
 * Se usar� para guardar en el game object una serie de punteros a funciones externas para ser llamadas en determinados momentos,
 * usando como referencia al objeto el par�metro "self".
 *
 * @param self Referencia a un objeto dado.
 */
typedef void (*function_t)(CGameObject* self);

/**
 * @brief Clase abstracta de un componente.
 *
 * Se usar� para representar y hacer de base a la hora de definir un componente.
 *
 * @see CGameObject.
 *
 * Un componente estar� asociado a un �nico objeto de juego, ignorando completamente componentes compartidos (imposible implementarlos por la asociaci�n bidireccional entre objeto y componente).
 *
 * Con respecto a la asociaci�n de los componentes con un objeto de juego, usamos polimorfimos para ello.
 * De cara a poder tener una gran variedad de componentes, ser�a l�gico usar un vector de �componentes� (o CComponentes) con el fin de almacenar nuestros componentes �nicos en un Game Object.
 *
 * @image html components/componentes.png
 *
 * Como se puede apreciar, cada componente tiene 3 partes:
 *
 * - Contenido o atributos del componente.
 * - M�todos del componente.
 * - Identificador �nico para ese componente.
 *
 * Para facilitar el acceso, puesto que un objeto de nuestro juego podr�a llegar a tener una cantidad elevada de componentes (dependiendo del n�mero de componentes implementados),
 * hemos decidido usar un std::map de la librer�a est�ndar de plantillas de C++. Adem�s, como se ha mencionado en el post anterior, un objeto s�lo puede tener un componente de cada tipo.
 * En este caso, para nuestro mapa, las claves ser�an unos identificadores num�ricos, mientras que el valor contenido ser�n punteros a dichos componentes,
 * que a su vez guardan un objeto al puntero que los contiene, intentando garantizar que los componentes no puedan ser compartidos
 * (tal vez interese implementar algo por el estilo m�s adelante).
 * Por tanto, tendremos un std::map con una lista de punteros del tipo CComponent que, por medio de polimorfismo y herencia,
 * crearemos una gama de clases hijas con distintas propiedades y ejecuciones de m�todos distintos. Se pueden realizar una serie de operaciones sobre los componentes desde
 * los Game Objects, entre ellas, las siguientes:
 *
 @code
    template
    Type* GetComponent();

    template
    bool AddComponent();

    template
    bool RemoveComponent();

    template
    void SetComponentStateComponent(bool state);

    // Funciones para acceso directo a los componentes:
    inline CComponent_Transform* transform()
    {
      // Siempre existe un componente Transform
      return (CComponent_Transform*)components[components::transform];
    }

    inline CComponent_Camera* camera()
    {
      if(components.find(components::camera) == components.end())
        components.insert(pair<int, CComponent*>(components::camera, new CComponent_Camera(this)));

      return (CComponent_Camera*)components[components::camera];
    }

  @endcode
  *
  * - Obtener componente: Devuelve un puntero al componente del tipo seleccionado. Si no existe, devolver� NULL.
  * - A�adir componente:  Intenta a�adir un componente. Si no existe, se crear� y devolver� true. Si existe, no se har� nada y devolver� false.
  * - Eliminar componente:  Intenta eliminar un componente. Si existe, lo elimina y devuelve true. Si no existe, no se har� nada y devolver� false.
  * - Cambiar estado:  Cambia el estado del componente (activado o desactivado) en funci�n del argumento state.
  * - Accesos directos: Permiten acceder de forma r�pida y c�moda al componente. Si el componente no existe, lo crea.
  *
  * Adem�s, se crear� un acceso directo en la clase GameObject (por ejemplo, CComponente_Transform* Transform() )
  * para cada componente, declarado, que actuar� de la siguiente forma: si el componente existe, devuelve un puntero.
  * Si no, crea el componente y lo devuelve. As� podemos garantizar que no se produzcan accesos indebidos a memoria por no comprobar la existencia de los componentes.
  *
  * @warning Un componente debe crearse usando los m�todos expuestos en la clase CGameObject. En principio, no deber�an usarse de manera expl�cita a la hora de operar (no hacemos un *new* de un componente).
 */
class CComponent
{
  friend class CGameObject;
  friend class CSystem_Debug;

  friend const char* Components::component_to_string(components_t c);
  friend Components::components_t  Components::string_to_component(const std::string& c);

  protected:
    bool enabled;
    CGameObject* gameObject;

    virtual void parseDebug(std::string command);
    virtual void printDebug();

  private:
    static int GetID() { return Components::base; }

    /*friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & enabled;
      //ar & id;
    }*/

  public:
    /**
     * @brief Constructor.
     */
    CComponent(){};
    /** Constructor con objeto asociado.
     *
     * Asocia al objeto pasado como argumento el componente creado.
     * @param container Objeto que guardar� el componente.
     */
    CComponent(CGameObject* container);

    /**
     * @brief Destructor.
     */
    virtual ~CComponent();

    /** @no_use */
    virtual void Set(input_t data){};
    /** @no_use */
    virtual output_t Get(){return NULL;};

    /**
     * @brief Renderizar el componente.
     *
     * Funci�n de renderizado. Renderiza el componente.
     *
     * @see CGameObject::OnRender().
     * @param projMatrix       Matriz de proyecci�n. Interna al sistema.
     * @param modelViewMatrix  Matriz de modelview. Interna al sistema.
     *
     * @warning Esta funci�n no debe ser llamada de manera expl�cita.
     */
    inline virtual void OnRender(glm::mat4 projMatrix, glm::mat4 modelViewMatrix){};
    /**
     * @brief Gesti�n del comportamiento.
     *
     * Funci�n de comportamiento. Actualiza el componente.
     *
     * @see CGameObject::OnLoop().
     *
     * @warning Esta funci�n no debe ser llamada de manera expl�cita.
     */
    inline virtual void OnLoop(){};
    /**
     * @brief Gesti�n de eventos.
     *
     * Funci�n de gesti�n de eventos. Gestiona eventos de entrada importantes para el componente.
     *
     * @see CGameObject::OnEvent().
     *
     * @warning Esta funci�n no debe ser llamada de manera expl�cita.
     */
    inline virtual void OnEvent(){};
    /**
     * @brief Gesti�n de entrada.
     *
     * Funci�n de gesti�n de entrada. Gestiona entradas importantes para el componente.
     *
     * @see CGameObject::OnInput().
     *
     * @warning Esta funci�n no debe ser llamada de manera expl�cita.
     */
    inline virtual void OnInput(){};

    //virtual bool AddFuncs(CGameObject* obj){return true;};
    //virtual bool RemoveFuncs(CGameObject* obj){return true;};

    /**
     * @brief Getter del estado del componente.
     *
     * Un componente puede estar marcado como **activado** como **desactivado**. En el primer caso, el componente se tratar� con normalidad, mientras
     * que en el segundo caso, en la mayor�a de componentes, se har� como que el componente no existe.
     *
     * @warning En algunos componentes es necesario que se actualicen algunos valores, por lo que el componente seguir� ejecutando ciertas funciones aunque est� desactivado.
     *
     * @return Retorna true si est� activado, false en caso contrario.
     */
    inline bool GetState()
    {
      return enabled;
    }

    /**
     * @brief Cambiar el estado del componente.
     *
     * Cambia el estado del componente actual por el estado especificado por "state" (true -> **activado**, false -> **desactivado**.
     *
     * @param state Nuevo estado del componente.
     */
    inline void SetState(bool state = true)
    {
      enabled = state;
    }

    /**
     * @brief Activa el componente.
     *
     * Cambia el estado del componente actual por **activado**.
     */
    inline void Enable()
    {
      enabled = true;
    }

    /**
     * @brief Desactiva el componente.
     *
     * Cambia el estado del componente actual por **desactivado**.
     */
    inline void Disable()
    {
      enabled = false;
    }

    /**
     * @brief Obtener el gameObject que encapsula este componente.
     *
     * Existe una relaci�n bidireccional entre los gameObjects y los componentes que tienen. Por tanto, puede ser de utilidad obtener
     * el objeto que tiene un componente actualmente, ya sea para acceder a algunos atributos del objeto en cuesti�n, o para acceder a otro componente.
     * @return Devuelve un puntero a un gameObject que tenga el componente. No deber�a ser NULL en ning�n momento.
     */
    inline CGameObject* GetGameObject()
    {
      return gameObject;
    }
};

/*@}*/

#endif /* __COMPONENT_H_ */
