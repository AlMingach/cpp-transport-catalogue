#pragma once

#include "transport_catalogue.h"

namespace load_queue {

	class Stop {
	public:

		Stop(std::string name, std::string latitude, std::string longitude);

		std::string& GetName();

		Coordinates& GetCoordinates();

	private:
		std::string name_;
		Coordinates coordinates_;
	};

	class StopDistance {
	public:

		StopDistance(std::string name, std::string text);

		std::string& GetName();

		std::unordered_map<std::string, int>& GetDistanceToStop();

	private:
		std::string name_;
		std::unordered_map<std::string, int> stops_distance_;
	};

	class Bus {
	public:

		Bus(std::string name, std::string text);

		std::string& GetName();

		std::vector<std::string>& GetStops();

		bool GetRound();

	private:
		std::string name_;
		std::vector<std::string> stops_;
		bool round_;

		void ParseStopName(std::string text, char c);

	};


	class Queue {
	public:

		Queue(std::istream& input);

		std::vector<Stop>& GetStops();

		std::vector<StopDistance>& GetStopDistance();

		std::vector<Bus>& GetBus();

	private:
		std::vector<Stop> stops_;
		std::vector<StopDistance> stops_distance_;
		std::vector<Bus> buses_;
	};

	bool IsValueText(std::string_view text);

	bool IsLatitude(double number);

	bool Islongitude(double number);

	void LoadInfo(TransportCatalogue& catalogue, Queue&& query);

}
