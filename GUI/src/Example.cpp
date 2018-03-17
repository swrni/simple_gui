#include <iostream>

#include "Window.hpp"


using namespace GUI;


int main() {
	Window window( sf::VideoMode( 300, 200 ), L"title" );
	sf::RenderWindow& render_window = window.getRenderWindow();
	
	const ButtonThemes themes;
	
	// Text edit.
	TextEdit* text_edit = window.CreateWidget<TextEdit>(
		sf::Vector2f{ 20, 90 },
		themes.events_to_themes_
	);
	window.MakeEditable( text_edit );
	
	// Exit button.
	Button* exit_button = window.CreateWidget<Button>(
		sf::Vector2f{ 20, 50 },
		themes.events_to_themes_
	);
	exit_button->setText( "Exit" );

	exit_button->CreateEventHandler<sf::RenderWindow*>(
		MouseEventType::kOnSelect,
		[]( const DataWrapperBase* base ) {
			auto derived = DataWrapper<sf::RenderWindow*>::Cast( base );
			auto render_window = derived->stored_object_;

			render_window->close();
		},
		&render_window
	);

	window.MakeSelectable( exit_button );

	// Test button.
	Button* test_button = window.CreateWidget<Button>(
		sf::Vector2f{ 20, 140 },
		themes.events_to_themes_
	);
	test_button->setText( "Test" );

	test_button->CreateEventHandler<Button*>(
		MouseEventType::kOnSelect,
		[]( const DataWrapperBase* base ) {
			auto derived = DataWrapper<Button*>::Cast( base );
			auto test_button = derived->stored_object_;

			std::cout << test_button << '\n';
			if ( test_button ) {
				test_button->setText( "test" );
			}
		},
		test_button
	);
	window.MakeSelectable( test_button );

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
	system( "pause" );
	return 0;
}