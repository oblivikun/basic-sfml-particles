#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <cmath>

struct Particle
{
    sf::Vector2f velocity;
    sf::Time lifetime;
};

class ParticleSystem : public sf::Drawable, public sf::Transformable
{
public:
    ParticleSystem(unsigned int count) :
    m_particles(count),
    m_vertices(sf::Points, count),
    m_lifetime(sf::seconds(3)),
    m_emitter(0, 0)
    {
    }

    void setEmitter(sf::Vector2f position)
    {
        m_emitter = position;
    }

    void update(sf::Time elapsed)
    {
        for (std::size_t i = 0; i < m_particles.size(); ++i)
        {
            // update the particle lifetime
            Particle& p = m_particles[i];
            p.lifetime -= elapsed;

            // if the particle is dead, respawn it
            if (p.lifetime <= sf::Time::Zero)
                resetParticle(i);

            // update the position of the corresponding vertex
            m_vertices[i].position += p.velocity * elapsed.asSeconds();

            // update the alpha (transparency) of the particle according to its lifetime
            float ratio = p.lifetime.asSeconds() / m_lifetime.asSeconds();
            m_vertices[i].color.a = static_cast<sf::Uint8>(ratio * 255);
        }
    }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const
    {
        // apply the transform
        states.transform *= getTransform();

        // our particles don't use a texture
        states.texture = NULL;

        // draw the vertex array
        target.draw(m_vertices, states);
    }

private:
    void resetParticle(std::size_t index)
    {
        // give a random velocity and lifetime to the particle
        float angle = (std::rand() % 360) * 3.14f / 180.f;
        float speed = (std::rand() % 50) + 50.f;
        m_particles[index].velocity = sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
        m_particles[index].lifetime = sf::milliseconds((std::rand() % 2000) + 1000);

        // reset the position of the corresponding vertex
        m_vertices[index].position = m_emitter;
    }

private:
    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
    sf::Time m_lifetime;
    sf::Vector2f m_emitter;
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "RGB Color Changing Text");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cout << "Error loading font\n";
        return 1;
    }

    sf::Text text("thats deep", font, 50);
    text.setPosition(100, 250);

    sf::Clock clock;

    // create the particle system
    ParticleSystem particles(103200);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();

        // Change the color of the text over time
        sf::Time elapsed = clock.getElapsedTime();
        sf::Color color(
            (sin(elapsed.asSeconds()) + 1.f) * 0.5f * 255,
            (sin(elapsed.asSeconds() + 2.f) + 1.f) * 0.5f * 255,
            (sin(elapsed.asSeconds() + 4.f) + 1.f) * 0.5f * 255
        );
        text.setFillColor(color);

        // update the particles
        particles.setEmitter(window.mapPixelToCoords(sf::Mouse::getPosition(window)));
        particles.update(elapsed);

        window.draw(text);
        // draw the particles
        window.draw(particles);

        window.display();
    }

    return 0;
}
