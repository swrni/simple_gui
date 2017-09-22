#include <iostream>

#include "Window.hpp"


using namespace GUI;

int main() {
	Window window( sf::VideoMode( 300, 200 ), L"title" );
	sf::RenderWindow& render_window = window.getRenderWindow();

	TextEdit* text_edit = window.CreateTextEdit( { 20, 90 } );
	
	Button* exit_button = window.CreateButton( { 20, 50 } );
	exit_button->setText( "Exit" );
	exit_button->CreateEventHandler(
		MouseEventType::kOnSelect,
		[]( const void* p_render_window ) {
			sf::RenderWindow* render_window = (sf::RenderWindow*)p_render_window;
			render_window->close();
		},
		&render_window
	);

	// Loop.
	while ( render_window.isOpen() ) {
		sf::Event event;
		while ( render_window.pollEvent( event ) ) {
			if ( event.type == sf::Event::Closed ) {
				render_window.close();
			}
			else if ( event.type == sf::Event::MouseMoved ) {
				window.OnMouseMoved();
			}
			else if ( event.type == sf::Event::MouseButtonPressed ) {
				window.OnMouseButtonPressed();
			}
			else if ( event.type == sf::Event::MouseButtonReleased ) {
				window.OnMouseButtonReleased();
			}
			else if ( event.type == sf::Event::TextEntered ) {
				window.OnKeyboardInput( event );
			}
		}

		// Clear and render everything.
		render_window.clear( sf::Color::White );
		window.Render();
		render_window.display();
	}

	return 0;
}