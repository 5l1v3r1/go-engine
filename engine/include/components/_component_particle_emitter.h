#ifndef __COMPONENT_PARTICLE_EMITTER_H_
#define __COMPONENT_PARTICLE_EMITTER_H_

#include "_globals.h"
#include "components/_component.h"

class CComponent_Particle_Emitter: public CComponent
{
  public:
    friend class CGameObject;

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
        vector3f_t scale, scale_factor;

        // �?
        //void OnRender();
        //void OnLoop();
    };

    vector<CParticle*> particles;

    void NewParticle(CParticle* p);

  public:
    /*void SetNumParticles(uint n)
    {
      if(n) particles.resize(n);
    }

    uint GetNumParticles()
    {
      return particles.size();
    }*/

    // Valores maximos y minimos iniciales (start values) y valores m�ximos y m�nimos por iteraci�n.

    // Se podr�an usar varios materiales, con una probabilidad P(X)c[0.f, 1.f] de que una particula use el material X
    uint max_particles;
    string material_name;

    // Usado para CSystem_Math::random_vector(direction, angle_spreed);
    //CSystem_Math::cone_t spread_cone;

    // Propiedades del "emisor" de part�culas
    bool freeze; // Congelar part�culas.
    bool stop;   // Dejar de emitir part�culas.

    // Values
    vector3f_t direction;
    GLfloat angle_spread;

    GLfloat max_vel, min_vel;
    GLfloat max_angle_vel, min_angle_vel;
    GLfloat max_scale, min_scale;
    //GLfloat max_live_time, min_live_time;

    // Start values
    GLfloat start_max_life_time, start_min_life_time;
    GLfloat start_max_distance, start_min_distance;
    GLfloat start_max_angle, start_min_angle;
    GLfloat start_max_angle_vel, start_min_angle_vel;
    GLfloat start_max_vel, start_min_vel;
    GLfloat start_max_scale, start_min_scale;
    GLfloat start_max_scale_factor, start_min_scale_factor;

    colorf_t color;
    vector3f gravity;

    colorf_t color_adder;

    // Para cambiar el nuevo color de las part�culas

  private:
    static int GetID() { return components::particle_emitter; }

    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
      ar & boost::serialization::base_object<CComponent>(*this);
    }

  public:
    CComponent_Particle_Emitter(){};
    CComponent_Particle_Emitter(CGameObject* gameObject);
    ~CComponent_Particle_Emitter();

    void Start();

    void Stop();
    void Resume();

    void Freeze();
    void UnFreeze();

    // Usamos glBegin() y glEnd() en vez de VBOs, ya que
    void OnRender();
    void OnLoop();
};

BOOST_CLASS_EXPORT_KEY( CComponent_Particle_Emitter );


#endif /* __COMPONENT_PARTICLE_EMITTER_H_ */
