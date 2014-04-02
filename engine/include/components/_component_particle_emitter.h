/**
 * @file
 * @brief Fichero que incluye la clase CComponent_Particle_Emitter
 */

#ifndef __COMPONENT_PARTICLE_EMITTER_H_
#define __COMPONENT_PARTICLE_EMITTER_H_

#include "_globals.h"
#include "components/_component.h"

// Ver http://www.opengl-tutorial.org/intermediate-tutorials/billboards-particles/particles-instancing/ para VBOs


/** @addtogroup Componentes */
/*@{*/

/**
 * @brief Componente emisor de part�culas
 *
 * El componente de emisi�n de part�culas se encarga de crear efectos visuales tales como pantallas de humo,
 * fuegos artificiales, disparos, etc.
 *
 * ��V�DEO AQU�!!
 *
 * Para ello, hemos optado por un dise�o bastante sencillo compuesto de 2 partes:
 *
 * - Una cantidad determinada de part�culas: cada una se tratar� como una entidad (OJO, entidad, no objeto)
 *    que tendr� su posici�n, su velocidad, su aceleraci�n, su velocidad angular, etc.
 * - Un contenedor que gestionar� dichas part�culas en cada iteraci�n del bucle principal,
 *    calculando sus nuevas propiedades (posici�n, velocidad, color, vida�) y mostr�ndolas
 *    por pantalla en funci�n de dichas propiedades.
 *
 * ��IMAGEN AQU�!!
 *
 * Una part�cula, en este caso, es una textura en el mundo 3D que siempre mira a la c�mara (billboard).
 * Por tanto, una part�cula se define con las siguientes propiedades:
 * - Una posici�n en el espacio.
 * - Una velocidad.
 * - Una aceleraci�n.
 * - Un �ngulo de rotaci�n.
 * - Una velocidad angular.
 * - Una aceleraci�n angular (no implementado).
 * - Una escala.
 * - Un factor de escala.
 * - Un color.
 * - Un material o textura.
 * - Un tiempo de vida.
 * - Etc.
 *
 * Estos valores vendr�n definidos por el componente que gestionar� dichas part�culas, con propiedades como:
 *
 * - Valores m�ximos y m�nimos actuales (restricciones).
 * - Valores m�ximos y m�nimos iniciales (aleatorio).
 * - Numero m�ximo de part�culas.
 * - Etc.
 *
 * Finalmente, el funcionamiento del emisor de part�culas es el siguiente:
 *
 * Para iniciar:
 @code
 Desde i = 0 hasta max_particles
     Crear part�cula i con propiedades aleatorias.
 @endcode
 *
 * Para mostrar por pantalla
 @code
 Para cada p en particles:Si (p.life > 0) saltamos al siguiente p
     Aplicar textura y color a opengl
     Trasladarse a p.position
     Hacer que la part�cula se genere como un sprite o billboard
     Rotar p.angle
     Escalar p.scale
     Dibujar p
 @code
 *
 * Para iterar:
 @code
 Si est� congelado (freeze), salir.
 Para cada p en particles:Incrementar p.position a partir de p.velocity
     Incrementar p.velocity a partir de p.acceleration
     // Idem para los �ngulos y similar para la escala, y otro tipo de valores (color, etc).
     Decrementar el tiempo de vida de p con el tiempo delta (diferencia de tiempo entre �ltima iteraci�n e iteraci�n actual).
     Si la vida es < 0 y no est� en modo parado (stop, sin generar nuevas part�culas)
       Generar nueva part�cula p.
 @endcode
 *
 * Dicho funcionamiento se realizar� de manera autom�tica desde el componente.
 *
 * @warning Algunos atributos est�n sin usar. V�ase c�digo fuente.
 *
 * @see http://en.wikipedia.org/wiki/Particle_system
 */
class CComponent_Particle_Emitter: public CComponent
{
  friend class CGameObject;
  friend class CSystem_Render;

  private:
    class CParticle
    {
      friend class CComponent_Particle_Emitter;

      private:
        bool active;
        GLfloat life;//, fade;

        colorf_t color;
        vector3f_t position, velocity, acceleration;
        GLfloat angle, angle_velocity, angle_acceleration;
        GLfloat scale;
        GLfloat scale_factor;

        //string material_name;
      public:
        CParticle();
        ~CParticle();
    };

    std::vector<CParticle> particles;

    static bool InitRenderVBO();
    static void CloseRenderVBO();

    void NewParticle(CParticle& p, vector3f go_pos);

    // Aux
    float new_particles;

      // VertexArray
    // Esto lo voy a intentar hacer est�tico, por lo que reducir� enormemente el n�mero de VBOs en escena
    static GLuint m_ParticlesVAO;

      // For all (divisor = 0)
    static GLuint m_ParticlesVBOVertices;

      // Per particle (divisor = 1)
    // Estar�a bien, si funciona, usar un mat3 para guardar esto (9 elementos), usando as� un �nico VBO
    static GLuint m_ParticlesVBOPosition;   // vec3
    static GLuint m_ParticlesVBOAngleScale; // vec2
    static GLuint m_ParticlesVBOColor;      // vec4

      // Used to store update info.
    std::vector<GLfloat> v_ParticlePosition_data;
    std::vector<GLfloat> v_ParticlesAngleScale_data;
    std::vector<GLfloat> v_ParticlesColor_data;

    void UpdateVBO();

    vector3f last_pos;

  protected:
    void parseDebug(std::string command);
    void printDebug();

  public:
    // Se podr�an usar varios materiales, con una probabilidad P(X)c[0.f, 1.f] de que una particula use el material X
    uint max_particles;            /**< Cantidad m�xima de part�culas a emitir. @warning No debe cambiarse una vez iniciado el emisor. Debe pararse y ejecutarse. */
    GLfloat particles_per_second;  /**< Ratio de emisi�n (part�culas por segundo). Puede cambiarse de manera din�mica. */
    std::string material_name;     /**< Nombre de la textura a usar para las part�culas. Puede cambiarse de manera din�mica. @see CSystem_Resources @see CResource_Texture */
    //vector<string> materials; // random materials, pero esto se podr�a conseguir con 2 emisores de part�culas...

    // Usado para CSystem_Math::random_vector(direction, angle_spreed);
    //CSystem_Math::cone_t spread_cone;

    // Propiedades del "emisor" de part�culas
    bool freeze; /**< Congelar part�culas (Inmovilizar, Inmortalizar). */
    bool stop;   /**< Dejar de emitir part�culas. Las part�culas vivas seguir�n su trayectoria hasta morir. */

    // Cono
    vector3f_t direction;          /**< Direcci�n de emisi�n de part�culas. Se usa para saber donde apunta la base del cono del emisor (plano de la base del cono perpendicular al vector, siendo un plano al mismo).*/
    GLfloat angle_spread;          /**< �ngulo de dispersi�n. Ampliaci�n del cono de generaci�n de part�culas para generar vectores aleatorios. Representa el �ngulo total, no desde un borde al centro. */

    GLfloat max_vel,               /**< M�xima velocidad de una part�cula mientras est� viva.*/
            min_vel;               /**< M�nima velocidad de una part�cula mientras est� viva. */
    GLfloat max_angle_vel,         /**< M�xima velocidad angular de una part�cula mientras est� viva. */
            min_angle_vel;         /**< M�nima velocidad angular de una part�cula mientras est� viva. */
    GLfloat max_scale,             /**< M�xima escala de una part�cula mientras est� viva. */
            min_scale;             /**< M�nima escala de una part�cula mientras est� viva. */
    //GLfloat max_live_time, min_live_time;
    colorf_t max_color,            /**< M�ximo color RGBA de una part�cula mientras est� viva. */
             min_color;            /**< M�nimo color RGBA de una part�cula mientras est� viva. */

    // Start values
    GLfloat start_max_life_time,
            start_min_life_time;
    GLfloat start_max_distance,
            start_min_distance;
    GLfloat start_max_angle,
            start_min_angle;
    GLfloat start_max_angle_vel,
            start_min_angle_vel;
    GLfloat start_max_vel,
            start_min_vel;
    GLfloat start_max_scale,
            start_min_scale;
    GLfloat start_max_scale_factor,
            start_min_scale_factor;

    //colorf_t color;
    colorf_t start_max_color,
             start_min_color;
    vector3f gravity;

    colorf_t color_adder;

    // Radio de la base
    GLfloat start_max_base_radius,
            start_min_base_radius;

  private:
    static int GetID() { return Components::particle_emitter; }

  public:
    CComponent_Particle_Emitter(){};
    CComponent_Particle_Emitter(CGameObject* gameObject);
    ~CComponent_Particle_Emitter();

    void Start();

    void Stop();
    void Resume();

    void Freeze();
    void UnFreeze();

  protected:
    void OnRender(glm::mat4 projMatrix, glm::mat4 modelViewMatrix);
    void OnLoop();
};

/*@}*/

#endif /* __COMPONENT_PARTICLE_EMITTER_H_ */
