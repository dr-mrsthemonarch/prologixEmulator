//
// Created by Kyle on 18.06.24.
//

#include "ClientList.h"
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <random>
#include "ftxui/component/component.hpp"
#include "ftxui/dom/elements.hpp"

class ClientList {
public:
    void CheckboxComponent();
    ftxui::Component GetRenderer();
    void Update();

    struct Order {
        bool state;
        std::string ipaddr;
    };

private:
    ftxui::Component order_book_component_;
    ftxui::Component table_container_;
    std::vector<ftxui::Component> rows_;
    std::vector<Order> bids_;
    std::vector<Order> asks_;
    std::chrono::steady_clock::time_point last_update_time_;

    std::mt19937 random_engine_;
    std::uniform_int_distribution<int> price_distribution_;
    std::uniform_int_distribution<int> volume_distribution_;

    void InitializeTable();
    void UpdateRowValuesIfNeeded();

    class TableRow : public ftxui::ComponentBase {
    public:
        TableRow(int index, std::vector<Order>* bids, std::vector<Order>* asks);
        ftxui::Element Render() final;
        bool Focusable() const final;

    private:
        int index_;
        std::vector<Order>* bids_;
        std::vector<Order>* asks_;
    };
};