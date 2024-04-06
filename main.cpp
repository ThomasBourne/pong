#define SFML_STATIC
#include <SFML/Graphics.hpp>
#include <array>

sf::Clock dtClock1;
sf::Clock dtClock2;
sf::Clock dtClock3;
int frameTime;
float lastTime = 0;

#define MAX_RATE 999

#define V_PADDLE_SPEED 1
#define MAX_BALL_SPEED 0.3f

#define AI true

//make sure key is given in uppercase
#define IS_KEY_PRESSED(key) sf::Keyboard::isKeyPressed(sf::Keyboard::##key)

int main() {
	//window handles
	sf::RenderWindow win(sf::VideoMode(1200, 800), "pong++", sf::Style::Titlebar | sf::Style::Close);
	win.setFramerateLimit(MAX_RATE);
	const float dt = 1.f / MAX_RATE;
	float accumulator = 0.f;
	bool drawn = false;
	sf::Event evnt;

	//game handles
	sf::Font f; //font
	f.loadFromFile("FONT_PATH");

	std::array<sf::RectangleShape, 2> players = { sf::RectangleShape(sf::Vector2f(20, 100)), sf::RectangleShape(sf::Vector2f(20, 100)) };
	players[0].setPosition(sf::Vector2f(0, win.getSize().y / 2 - (players[0].getSize().y / 2)));
	players[1].setPosition(sf::Vector2f(win.getSize().x-players[1].getSize().x, win.getSize().y / 2 - (players[1].getSize().y / 2)));
	std::array<int, 2> playerScores{ 0, 0 };
	std::array<sf::Text, 2> playerScoreText;
	for (auto& text : playerScoreText) {
		text.setFont(f);
		text.setString("0");
		text.setPosition(sf::Vector2f(20, 20));
	}
	playerScoreText[1].setPosition(sf::Vector2f(win.getSize().x - 20, 20));

	sf::RectangleShape ball(sf::Vector2f(20, 20));
	sf::Vector2f ballVeclocity{ -MAX_BALL_SPEED, 0.01f };
	ball.setPosition(sf::Vector2f(win.getSize().x / 2 - (ball.getSize().x / 2), win.getSize().y / 2 - (ball.getSize().y / 2)));

	sf::RectangleShape net(sf::Vector2f(5, win.getSize().y));
	net.setPosition(sf::Vector2f(win.getSize().x / 2 - 2.5f, 0));

	while (win.isOpen()) {
		while (win.pollEvent(evnt)) {
			if (evnt.type == sf::Event::Closed)
				win.close();
		}
		frameTime = dtClock1.restart().asMilliseconds();

		accumulator += dtClock2.getElapsedTime().asSeconds();
		dtClock2.restart();

		while (accumulator >= dt) {
			//physics updates

			//check for input from player 1
#if not AI
				if (IS_KEY_PRESSED(W)) {
					players[0].move(sf::Vector2f(0, -V_PADDLE_SPEED));
					if (players[0].getPosition().y < 0)
						players[0].setPosition(sf::Vector2f(players[0].getPosition().x, 0));
				}
				else if (IS_KEY_PRESSED(S)) {
					players[0].move(sf::Vector2f(0, V_PADDLE_SPEED));
					if (players[0].getPosition().y + players[0].getSize().y > win.getSize().y)
						players[0].setPosition(sf::Vector2f(players[0].getPosition().x, win.getSize().y - players[0].getSize().y));
				}
#else
				if (players[0].getPosition().y + players[0].getSize().y > ball.getPosition().y)
					players[0].move(sf::Vector2f(0, -V_PADDLE_SPEED));
				if(players[0].getPosition().y < ball.getPosition().y + ball.getSize().y - 1.f)
					players[0].move(sf::Vector2f(0, V_PADDLE_SPEED));
#endif

			//check for input from player 2
			if (IS_KEY_PRESSED(Up)) {
				players[1].move(sf::Vector2f(0, -V_PADDLE_SPEED));
				if (players[1].getPosition().y < 0)
					players[1].setPosition(sf::Vector2f(players[1].getPosition().x, 0));
			}
			else if (IS_KEY_PRESSED(Down)) {
				players[1].move(sf::Vector2f(0, V_PADDLE_SPEED));
				if (players[1].getPosition().y + players[1].getSize().y > win.getSize().y)
					players[1].setPosition(sf::Vector2f(players[1].getPosition().x, win.getSize().y - players[1].getSize().y));
			}

			//check for ball collision and change its velocity if so
			if (players[0].getGlobalBounds().intersects(ball.getGlobalBounds())) { //ball hit p1
				float min = players[0].getPosition().y;
				float max = players[0].getPosition().y + players[0].getSize().y;
				float ballPos = ball.getPosition().y + (ball.getSize().y / 2);
				ballVeclocity.x = -ballVeclocity.x;
				//https://math.stackexchange.com/questions/914823/shift-numbers-into-a-different-range
				//f(t) = c + ( (d-c) / (b-a) ) * (t-a)
				ballVeclocity.y = -MAX_BALL_SPEED + ((MAX_BALL_SPEED - (-MAX_BALL_SPEED)) / (max - min) * (ballPos - min));
				ball.setPosition(sf::Vector2f(players[0].getPosition().x + players[0].getSize().x + 0.1f, ball.getPosition().y)); //prevent catching on paddle
			}
			else if (players[1].getGlobalBounds().intersects(ball.getGlobalBounds())) { //ball hit p2
				float min = players[1].getPosition().y;
				float max = players[1].getPosition().y + players[1].getSize().y;
				float ballPos = ball.getPosition().y + (ball.getSize().y / 2);
				ballVeclocity.x = -ballVeclocity.x;
				//https://math.stackexchange.com/questions/914823/shift-numbers-into-a-different-range
				//f(t) = c + ( (d-c) / (b-a) ) * (t-a)
				ballVeclocity.y = -MAX_BALL_SPEED + ((MAX_BALL_SPEED - (-MAX_BALL_SPEED)) / (max - min) * (ballPos - min));
				ball.setPosition(sf::Vector2f(players[1].getPosition().x - ball.getSize().x - 0.1f, ball.getPosition().y)); //prevent catching on paddle
			}

			ball.move(ballVeclocity);
			//check ball is in bounds of game
			if (ball.getPosition().y < 0) {
				ball.setPosition(sf::Vector2f(ball.getPosition().x, 0));
				ballVeclocity.y = -ballVeclocity.y;
			}
			if (ball.getPosition().y + ball.getSize().y > win.getSize().y) {
				ball.setPosition(sf::Vector2f(ball.getPosition().x, win.getSize().y - ball.getSize().y));
				ballVeclocity.y = -ballVeclocity.y;
			}

			//check for ball going past a paddle
			if (ball.getPosition().x < 0) { //p1 lost
				playerScores[1]++;
				playerScoreText[1].setString(std::to_string(playerScores[1]));

				ball.setPosition(sf::Vector2f(win.getSize().x / 2 - (ball.getSize().x / 2), win.getSize().y / 2 - (ball.getSize().y / 2)));
				ballVeclocity.x = -ballVeclocity.x;
				ballVeclocity.y = -0.01f;
			}
			else if (ball.getPosition().x + ball.getSize().x > win.getSize().x) { //p2 lost
				playerScores[0]++;
				playerScoreText[0].setString(std::to_string(playerScores[0]));

				ball.setPosition(sf::Vector2f(win.getSize().x / 2 - (ball.getSize().x / 2), win.getSize().y / 2 - (ball.getSize().y / 2)));
				ballVeclocity.x = -ballVeclocity.x;
			}

			accumulator -= dt;
			drawn = false;
		}

		if (drawn)
		{
			sf::sleep(sf::microseconds(1));
		}
		else
		{
			float currentTime = dtClock3.restart().asSeconds();
			unsigned short fps = 1 / currentTime;
			lastTime = currentTime;

			// Draw everything.
			win.clear();
			sf::Text t{ std::to_string(fps), f};
			for (const auto& player : players)
				win.draw(player);
			win.draw(ball);
			for(const auto& score : playerScoreText)
				win.draw(score);
			win.draw(net);
			win.draw(t);
			win.display();

			drawn = true;
		}
	}
}