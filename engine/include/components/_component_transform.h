/**
 * @file
 * @brief Fichero que incluye la clase CComponent_Transform.
 */

#ifndef __COMPONENT_TRANSFORM_H_
#define __COMPONENT_TRANSFORM_H_

#include "_globals.h"
#include "systems/_other.h"
#include "components/_component.h"

/** @addtogroup Componentes */
/*@{*/

/**
 * @brief Componente de transformaci�n.
 *
 * El componente de transformaci�n representa la posici�n, la orientaci�n y la escala de nuestro objeto en el mundo.
 *
 * @image html components/transform/transform.jpg
 *
 * En un principio, se representaban estos 3 vectores con un vector de 3 componentes flotantes, tales como:
 @code
 vector3f position;
 vector3f angle;
 vector3f scale;
 @endcode
 * Esto nos permite operar de forma sencilla sobre esos valores, con operaciones del tipo traslaci�n, rotaci�n y escalado (locales al eje actual del objeto, y globales (con respecto al eje X, Y, Z).
 *
 * No obstante, la representaci�n de estos valores (en concreto, la orientaci�n con "angle") daba una serie de problemas.
 * Uno de ellos era un manejo lento, complicado, pesado y poco eficiente. De cara al usuario, trabajar con grados sobre los ejes X, Y, Z
 * (�ngulos de Euler) era muy c�modo, pero de cara al sistema, lento. Adem�s, hay un fen�meno muy molesto llamado "Gimbal lock",
 * que, dada una combinaci�n de rotaciones en los ejes X, Y, Z, se anula una componente de las rotaciones (es decir, rotando en 2 ejes distintos,
 * el objeto rota sobre el mismo eje en nuestro mundo). Como esto es chapuza problem�tica, he decidido buscar una mejor soluci�n.
 *
 * @image html components/transform/gimbal-2.gif
 *
 * En todos los sitios relacionados al desarrollo de videojuegos hablaban de los cuaterniones, y lo
 * peor es que hablaban mal de ellos, como una pesadilla, lo cual me empujaba a buscar otras soluciones.
 * Despu�s de numerosos intentos fallidos, he decidido usar los cuaterniones para la representaci�n interna de la orientaci�n de un objeto.
 * As�, evitamos el efecto �Gimbal lock� y ganamos en eficiencia para hacer rotaciones (simplemente toca multiplicar la matriz MODELVIEW actual por la matriz generada por el cuaterni�n actual).
 *
 * No obstante, como esta representaci�n es bastante confusa, tenemos una simple soluci�n:
 *
 * - De cara al sistema, se trabajar� con cuaterniones con pura eficiencia.
 * - De cara al usuario (el que programar� sus juegos), se le dar� la posibilidad de trabajar con �ngulos de Euler
 *    (intuitivos y lentos) o cuaterniones (r�pidos). No obstante, en ambos casos se trabajar� de manera indirecta con el segundo sistema
 *    (en el primer caso, se pasar� de �ngulos de Euler a cuaterniones).
 *
 * Como estos c�lculos son complejos, hemos decidido ampliar nuestro repositorio de librer�as y utilizar "GLM"
 * (Graphics Library Maths), una librer�a de s�lo cabeceras para hacer operaciones matem�ticas con gr�ficos 3D (lo usaremos para los cuaterniones, b�sicamente).
 * Seguramente, usemos varias funciones de esta librer�a para "inflar" nuestro sistema "CSystem_Math".
 *
 * Por ende, ahora se representar� el componente como:
 @code
 vector3f position;
 vector3f scale;
 glm::quat angle;
 @endcode
 *
 * @see http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
 * @see http://en.wikipedia.org/wiki/Gimbal_lock
 * @see http://es.wikipedia.org/wiki/%C3%81ngulos_de_Euler
 * @see http://glm.g-truc.net/0.9.5/index.html
 */
class CComponent_Transform: public CComponent
{
  friend class CGameObject;
  friend class CSystem_Render;
  friend class CSystem_Debug;

  friend const char* Components::component_to_string(components_t c);
  friend Components::components_t Components::string_to_component(const std::string& c);

  public:
    vector3f position; /**< Posici�n en el espacio tridimensional. Es la posici�n **local** con respecto al padre del objeto que tiene el componente. La posici�n global vendr� dada por la aplicaci�n de m�todos como ApplyParentTransform(), entre otras operaciones. */
    vector3f scale;    /**< Escala en el espacio tridimensional. Es la escala **local** con respecto al padre del objeto que tiene el componente. La escala global vendr� dada por la aplicaci�n de m�todos como ApplyParentTransform(), entre otras operaciones. */
    glm::quat angle;   /**< Rotaci�n en el espacio tridimensional, representada por un cuaterni�n. Es la rotaci�n **local** con respecto al padre del objeto que tiene el componente. La escala global vendr� dada por la aplicaci�n de m�todos como ApplyParentTransform(), entre otras operaciones. @warning Se recomienda no modificar este valor de manera directa a no ser que se sepa lo que est� haciendo. Use SetAngle(), LRotation(), Rotate() en su defecto. @see http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/ */

  private:
    static GLuint m_TransformVBOVertices;
    static GLuint m_TransformVBOColors;
    static GLuint m_TransformVAO;

    static int GetID() { return Components::transform; }

    void parseDebug(std::string command);
    void printDebug();

  protected:
    void ApplyParentTransform(CGameObject* parent, glm::mat4& transformMatrix);

  public:
    /**
     * @brief Constructor vac�o.
     */
    CComponent_Transform(){};
    /** @brief Constructor con objeto asociado.
     *
     * Asocia al objeto pasado como argumento el componente creado. Adem�s, inicializa los atributos de la clase a unos ciertos valores:
     *
     @code
     position.x = position.y = position.z = 0;
     scale.x = scale.y = scale.z = 1.f;
     // cuaterni�n iniciado por glm (sin rotaci�n).
     @endcode
     *
     * @param gameObject Objeto que guardar� el componente.
     */
    CComponent_Transform(CGameObject* gameObject);
    /**
     * @brief Destructor.
     *
     * Destruye el componente.
     */
    ~CComponent_Transform();

    /**
     * @brief @no_use
     *
     * @no_use
     * @param data
     */
    inline virtual void Set(input_t data);
    /**
     * @brief @no_use
     *
     * @no_use
     * @return
     */
    inline virtual output_t Get();

  public:
    // Movimientos locales o globales al eje x,y,z
    // Cambia mucha la cosa:
    //    Por ejemplo, mover rotar un objeto con hijos... �d�nde acaban los hijos?
    //      -> Posici�n de los hijos LOCAL al padre (calculada o guardada) <- DECISI�N
    //          -> Se deber� calcular de manera recursiva hacia arriba hasta llegar al mayor padre sin hijos.
    //          -> No hace falta calcular la posici�n y luego hacer la transformaci�n
    //              -> Se realizar�n las transformaciones recursivamente desde arriba hasta abajo, sin calcular posiciones
    // �qu� tal un booleano para saber cuando se ha renderizado un hijo? As�, las transformaciones se realizan en cascada de arriba a abajo, pasando por todos los hijos
    //    -> Problematico? Y si no usamos DEPTH_TEST? Mejor dejarlo as�

    // Transformaciones locales �?
    /**
     * @brief Traslaci�n local.
     *
     * Traslada el objeto de manera local a su padre, vi�ndose afectada por la transformaci�n de sus ancestros.
     * En caso de no tener padre, se considerar� traslaci�n global.
     *
     * @param x Unidades a trasladar en el eje x local al objeto.
     * @param y Unidades a trasladar en el eje y local al objeto.
     * @param z Unidades a trasladar en el eje z local al objeto.
     */
    void LTranslate(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0);
    /**
     * @brief Rotaci�n local.
     *
     * Rota el objeto de manera local a su padre, vi�ndose afectada por la transformaci�n de sus ancestros.
     * En caso de no tener padre, se considerar� rotaci�n global.
     *
     * @param x Unidades a rotar en el eje x local al objeto, expresada en grados.
     * @param y Unidades a rotar en el eje y local al objeto, expresada en grados.
     * @param z Unidades a rotar en el eje z local al objeto, expresada en grados.
     */
    void LRotate(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0); // <- ANGULOS DE EULER Y CUATERNIONES!!!
    //void LScale(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0);

    //vector3f AxisAngles();
    /**
     * @brief �ngulos de euler.
     *
     * Devuelve una representaci�n, en �ngulos de Euler, del cuaterni�n almacenado en angle.
     *
     * @warning Se recomienda usar LRotation() en vez de EulerAngles(), ya que es m�s intuitivo.
     *
     * @return Valor almacenado en angle como un vector de 3 componentes, siendo los �ngulos de Euler.
     *
     * @see http://es.wikipedia.org/wiki/%C3%81ngulos_de_Euler
     */
    vector3f EulerAngles();

    // Problem�tica: Rotaciones locales y globales: las rotaciones "globales" funcionan como pseudolocales
    //  -> Usar cuaterniones, ya que la rotaci�n de un eje altera el resto de ejes.
    // Problem�tica: Gimbal Lock
    //  -> Usar cuaterniones en vez de �ngulos de eulers, y mostrar al usuario los �ngulos como �ngulos de euler
    //  -> Set: Euler a Cuaterniones
    //  -> Get: Cuaterniones a Euler
    //  -> Se trabaja internamente con cuaterniones. No se podr� modificar los �ngulos de euler a mano (angle.x ...), ya que internamente no existen. Se usar� el set y el get exclusivamente.
    //  -> http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-17-quaternions/
    //     - Nota: se puede usar glRotatef con un cuaternion?

    /**
     * @brief Traslaci�n global.
     *
     * Traslada el objeto de manera global, movi�ndose sobre los ejes x,y,z globales (movimiento libre por el mundo), con respecto al padre.
     *
     * @param v Vector de unidades a trasladar en los ejes globales.
     */
    void Translate(vector3f v);
    /**
     * @brief Traslaci�n global.
     *
     * Traslada el objeto de manera global, movi�ndose sobre los ejes x,y,z globales (movimiento libre por el mundo), con respecto al padre.
     *
     * @param x Unidades a trasladar en el eje x global.
     * @param y Unidades a trasladar en el eje y global.
     * @param z Unidades a trasladar en el eje z global.
     */
    void Translate(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0);
    /**
     * @brief Cambiar posici�n global, con respecto al padre.
     *
     * Cambia la posici�n del objeto, que depende de la transformaci�n de sus ancestros. Por ejemplo, si se cambiase a *(0, 0, 0)*, se colocar�a en el mismo sitio en el que su padre.
     * En caso de no tener padre, se considerar� posici�n global.
     *
     * @param v Vector que representa la nueva posici�n local al padre.
     */
    void SetPosition(vector3f v);
    /**
     * @brief Cambiar posici�n global, con respecto al padre.
     *
     * Cambia la posici�n del objeto, que depende de la transformaci�n de sus ancestros. Por ejemplo, si se cambiase a *(0, 0, 0)*, se colocar�a en el mismo sitio en el que su padre.
     * En caso de no tener padre, se considerar� posici�n global.
     *
     * @param x Nueva posici�n en el eje x, local al padre.
     * @param y Nueva posici�n en el eje y, local al padre.
     * @param z Nueva posici�n en el eje z, local al padre.
     */
    void SetPosition(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0);
    /**
     * @brief Rotaci�n global.
     *
     * Rota el objeto de manera global, rotando sobre los ejes x,y,z globales (movimiento libre por el mundo), con respecto al padre.
     *
     * @param v Vector de unidades a rotar en los ejes globales, con componentes expresadas en grados.
     */
    void Rotate(vector3f v);
    /**
     * @brief Rotaci�n global.
     *
     * Rota el objeto de manera global, rotando sobre los ejes x,y,z globales (movimiento libre por el mundo), con respecto al padre.
     *
     * @param x Unidades a rotar en el eje x global, expresadas en grados.
     * @param y Unidades a rotar en el eje y global, expresadas en grados.
     * @param z Unidades a rotar en el eje z global, expresadas en grados.
     */
    void Rotate(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0);
    /**
     * @brief Cambiar rotaci�n  global, con respecto al padre.
     *
     * Cambia la rotaci�n del objeto, que depende de la rotaci�n de sus ancestros. Por ejemplo, si se cambiase a *(0, 0, 0)*, se rotar�a a la misma orientaci�n que su padre.
     * En caso de no tener padre, se considerar� rotaci�n global.
     *
     * @param v Vector que representa la nueva rotaci�n local al padre.
     */
    void SetAngle(vector3f v);
    /**
     * @brief Cambiar rotaci�n  global, con respecto al padre.
     *
     * Cambia la rotaci�n del objeto, que depende de la rotaci�n de sus ancestros. Por ejemplo, si se cambiase a *(0, 0, 0)*, se rotar�a a la misma orientaci�n que su padre.
     * En caso de no tener padre, se considerar� rotaci�n global.
     *
     * @param x Nueva rotaci�n en el eje x, local al padre, expresada en grados.
     * @param y Nueva rotaci�n en el eje y, local al padre, expresada en grados.
     * @param z Nueva rotaci�n en el eje z, local al padre, expresada en grados.
     */
    void SetAngle(GLfloat x = 0, GLfloat y = 0, GLfloat z = 0);
    /**
     * @brief Escala global.
     *
     * Escala el objeto de manera global, escalando sobre los ejes x,y,z globales (movimiento libre por el mundo), con respecto al padre.
     *
     * @warning La escala no es aditiva, sino multiplicativa. Si se escala el valor *1.5* por *2*, tendremos *3*, y no *3.5*.
     *
     * @param v Vector de unidades a escalar en los ejes globales.
     */
    void Scale(vector3f v);
    /**
     * @brief Escala global.
     *
     * Escala el objeto de manera global, escalando sobre los ejes x,y,z globales (movimiento libre por el mundo), con respecto al padre.
     *
     * @warning La escala no es aditiva, sino multiplicativa. Si se escala el valor *1.5* por *2*, tendremos *3*, y no *3.5*.
     *
     * @param x Unidades a escalar en el eje x global.
     * @param y Unidades a escalar en el eje y global.
     * @param z Unidades a escalar en el eje z global.
     */
    void Scale(GLfloat x = 1.f, GLfloat y = 1.f, GLfloat z = 1.f);
    /**
     * @brief Cambiar escala  global, con respecto al padre.
     *
     * Cambia la escala del objeto, que depende de la transformaci�n de sus ancestros. Por ejemplo, si se cambiase a *(1, 1, 1)*, se escalar�a a igual escala que su padre.
     * En caso de no tener padre, se considerar� escala global.
     *
     * @param v Vector que representa la nueva escala local al padre.
     */
    void SetScale(vector3f v);
    /**
     * @brief Cambiar escala  global, con respecto al padre.
     *
     * Cambia la escala del objeto, que depende de la transformaci�n de sus ancestros. Por ejemplo, si se cambiase a *(1, 1, 1)*, se escalar�a a igual escala que su padre.
     * En caso de no tener padre, se considerar� escala global.
     *
     * @param x Nueva escala en el eje x, local al padre.
     * @param y Nueva escala en el eje y, local al padre.
     * @param z Nueva escala en el eje z, local al padre.
     */
    void SetScale(GLfloat x = 1.f, GLfloat y = 1.f, GLfloat z = 1.f);

    /**
     * @brief Apuntar a una posici�n
     *
     * Dado un punto en el espacio (global), se ajustar� el objeto para que sus componentes apunten al punto determinado.
     *
     * @warning Se modificar�n todos los ejes del objeto para apuntar. En caso de no querer usar uno, se debe deshacer de forma
     * manual con SetAngle(), quitando la/s componente/s deseada/s, y dejando las que resten.
     *
     * @param target Punto en el espacio, representado como un vector *(x, y, z)*.
     * @param up_vector Vector que apunta hacia arriba. Por defecto, es CSystem_Math::Y_AXIS (hacia arriba). Puede usarse el vector up() del componente actual para mejores resultados.
     * @param forward_vector Vector que apunta hacia delante. Por defecto, es CSystem_Math::Z_Axis (hacia delante).  Puede usarse el vector forward() del componente actual para mejores resultados.
     */
    void LookAt(vector3f target, vector3f up_vector = gMath.Y_AXIS, vector3f forward_vector = gMath.Z_AXIS);
    /**
     * @brief Apuntar a una posici�n
     *
     * Dado un punto en el espacio (global), se ajustar� el objeto para que sus componentes apunten al punto determinado.
     *
     * @warning Se modificar�n todos los ejes del objeto para apuntar. En caso de no querer usar uno, se debe deshacer de forma
     * manual con SetAngle(), quitando la/s componente/s deseada/s, y dejando las que resten.
     *
     * @param x Coordenada x del punto a mirar.
     * @param y Coordenada y del punto a mirar.
     * @param z Coordenada z del punto a mirar.
     * @param up_vector Vector que apunta hacia arriba. Por defecto, es CSystem_Math::Y_AXIS (hacia arriba). Puede usarse el vector up() del componente actual para mejores resultados.
     * @param forward_vector Vector que apunta hacia delante. Por defecto, es CSystem_Math::Z_Axis (hacia delante).  Puede usarse el vector forward() del componente actual para mejores resultados.
     */
    void LookAt(GLfloat x, GLfloat y, GLfloat z, vector3f up_vector = gMath.Y_AXIS, vector3f forward_vector = gMath.Z_AXIS);

    /**
     * @brief Aplicar transformaci�n a una matriz de transformaci�n.
     *
     * Aplica los datos del componente (posici�n, rotaci�n y escala, en ese orden) y la de sus ancentros, empezando por el primero, y
     * acabando en el objeto actual, para generar una matriz modelview, usada internamente por openGL.
     *
     * No es necesario usarla, pero puede ser �til en algunos casos muy particulares.
     *
     * @param modelviewMatrix Matriz actual (por ejemplo, generada por la c�mara).
     * @return Devuelve la matriz "modelviewMatrix" transformada por las componentes descritas anteriormente.
     */
    glm::mat4 ApplyTransform(const glm::mat4& modelviewMatrix);

    /**
     * @brief Normalizar �ngulos.
     *
     * Normaliza los �ngulos.
     *
     * @deprecated Al usar cuaterniones, esta funci�n deja de tener sentido. No es conveniente usarla en ning�n momento.
     */
    inline void NormalizeAngles()
    {
      if(angle.x < 0 ) angle.x = 360 + angle.x;
      if(angle.y < 0 ) angle.y = 360 + angle.y;
      if(angle.z < 0 ) angle.z = 360 + angle.z;

      if(angle.x >= 360 ) angle.x = angle.x - 360;
      if(angle.y >= 360 ) angle.y = angle.y - 360;
      if(angle.z >= 360 ) angle.z = angle.z - 360;
    }

    /**
     * @brief Vector hacia "arriba" del componente.
     *
     * Dada una rotaci�n por angle, se generar� un vector que simule la direcci�n "arriba" para ese objeto rotado (eje y local).
     *
     * @bug No se tiene en cuenta a los padres para este c�lculo.
     *
     * @return Vector *(0.f, 1.f, 0.f)* rotado por la rotaci�n scale.
     */
    vector3f up();
    /**
     * @brief Vector hacia la "izquierda" del componente.
     *
     * Dada una rotaci�n por angle, se generar� un vector que simule la direcci�n "izquierda" para ese objeto rotado (eje x local).
     *
     * @bug No se tiene en cuenta a los padres para este c�lculo.
     *
     * @return Vector *(1.f, 0.f, 0.f)* rotado por la rotaci�n scale.
     */
    vector3f left();
    /**
     * @brief Vector hacia "adelante" del componente.
     *
     * Dada una rotaci�n por angle, se generar� un vector que simule la direcci�n "adelante" para ese objeto rotado (eje z local).
     *
     * @bug No se tiene en cuenta a los padres para este c�lculo.
     *
     * @return Vector *(0.f, 0.f, 1.f)* rotado por la rotaci�n scale.
     */
    vector3f forward();

    /**
     * @brief Alabeo.
     *
     * Retorna la rotaci�n local en el eje Z.
     *
     * @image html components/transform/pyr1.jpg
     *
     * @image html components/transform/pyr2.jpg
     *
     * @return �ngulo, expresado en grados, de la rotaci�n del eje Z del componente.
     */
    GLfloat pitch();
    /**
     * @brief Gui�ada.
     *
     * Retorna la rotaci�n local en el eje Y.
     *
     * @image html components/transform/pyr1.jpg
     *
     * @image html components/transform/pyr2.jpg
     *
     * @return �ngulo, expresado en grados, de la rotaci�n del eje Y del componente.
     */
    GLfloat yaw();
    /**
     * @brief Cabeceo.
     *
     * Retorna la rotaci�n local en el eje X.
     *
     * @image html components/transform/pyr1.jpg
     *
     * @image html components/transform/pyr2.jpg
     *
     * @return �ngulo, expresado en grados, de la rotaci�n del eje X del componente.
     */
    GLfloat roll();

    /**
     * @brief Rotaci�n global.
     *
     * Devuelve la rotaci�n global del objeto, aplicando las rotaciones de los padres (si los hubiese).
     *
     * @return Vector de 3 componentes, expresadas en grados, de la rotaci�n global del objeto.
     */
    vector3f Rotation();
    /**
     * @brief Rotaci�n local.
     *
     * Devuelve la rotaci�n local del objeto a su padre. Si no hay padre, la rotaci�n se considerar� global.
     * Este m�todo es id�ntico a obtener pitch(), yaw() y roll().
     *
     * @return Vector de 3 componentes, expresadas en grados, de la rotaci�n local del objeto.
     */
    vector3f LRotation();  // Pitch, yaw, roll

    /**
     * @brief Posici�n global.
     *
     * Devuelve la posici�n global del objeto, aplicando las transformaciones de los padres (si los hubiese).
     *
     * @return Vector de 3 componentes de la posici�n global del objeto.
     */
    vector3f_t Position();
    /**
     * @brief Posici�n local.
     *
     * Devuelve la posici�n local del objeto a su padre. Si no hay padre, la posici�n se considerar� global.
     * Es id�ntico a acceder al atributo position.
     *
     * @return Vector de 3 componentes de la posici�n local del objeto.
     */
    vector3f_t LPosition()
    {
      return position;
    }

    //vector3f_t Scalation();
    // vector3f_t LScalation();

  protected:
    void OnRender(glm::mat4 modelViewMatrix, glm::mat4 projMatrix);
    static bool InitRenderVBO();
    static void CloseRenderVBO();
};

/*@}*/

#endif /* __COMPONENT_TRANSFORM_H_ */
