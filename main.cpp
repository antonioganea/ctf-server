#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

void centerSpriteOrigin( sf::Sprite & sprite ){
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width/2.f, bounds.height/2.f);
}

void SpriteOriginToCenteredTop( sf::Sprite & sprite ){
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width/2.f, 0);
}

class VisualLock : public sf::Drawable{
    sf::Texture lockBodyTex;
    sf::Texture lockHeadTex;
    sf::Sprite lockBody;
    sf::Sprite lockHead;
    bool locked = true;
public:
    VisualLock(){
        lockBodyTex.loadFromFile("assets/lock_body.png");
        lockHeadTex.loadFromFile("assets/lock_head.png");
        lockBody.setTexture(lockBodyTex);
        lockHead.setTexture(lockHeadTex);
        SpriteOriginToCenteredTop(lockHead);
        SpriteOriginToCenteredTop(lockBody);
    }

    void draw(sf::RenderTarget & target, sf::RenderStates renderstates) const {
        target.draw(lockHead);
        target.draw(lockBody);
    }

    void setPosition(int x, int y){
        lockBody.setPosition(x,y);
        lockHead.setPosition( x, y-lockHead.getLocalBounds().height + 160);
    }

    void unlock(){
        if ( locked ){
            lockHead.move(0,-110);
            locked = false;
        }
    }
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "ctf-server", sf::Style::Fullscreen);

    window.setFramerateLimit(30);

    sf::Color lockedColor(210, 62, 53);
    sf::Color unlockedColor(30, 227, 0);

    VisualLock lock;
    sf::Vector2u winSize = window.getSize();
    lock.setPosition(winSize.x/2, winSize.y/2);

    sf::Font bariol;
    bariol.loadFromFile("assets/bariol.otf");

    sf::Text iptext;
    iptext.setFont(bariol);
    iptext.setCharacterSize(128);

    int port = 55001;

    std::string iptextstr = sf::IpAddress::getLocalAddress().toString();
    iptextstr += ":";
    iptextstr += std::to_string(port);

    iptext.setString(iptextstr);

    sf::FloatRect ipbounds = iptext.getLocalBounds();
    iptext.setPosition(winSize.x/2-ipbounds.width/2,0);
    bool locked = true;

    // Puzzle part
    sf::TcpListener listener;
    listener.listen(port);
    listener.setBlocking(false);

    sf::TcpSocket client;
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed){
                if ( event.key.code == sf::Keyboard::Escape)
                    window.close();
                /*
                if ( event.key.code == sf::Keyboard::Space){
                  lock.unlock();locked = false;
                }
                */
            }
        }

        if (listener.accept(client) == sf::Socket::Done){
            lock.unlock();locked = false;
        }

        if ( locked )
            window.clear(lockedColor);
        else
            window.clear(unlockedColor);
        window.draw(lock);
        window.draw(iptext);
        window.display();
    }
    return 0;
}
