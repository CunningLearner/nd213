#include "route_planner.h"
#include <algorithm>

RoutePlanner::RoutePlanner(RouteModel &model, float start_x, float start_y, float end_x, float end_y): m_Model(model) {
    // Convert inputs to percentage:
    start_x *= 0.01;
    start_y *= 0.01;
    end_x *= 0.01;
    end_y *= 0.01;

	this->start_node = &m_Model.FindClosestNode(start_x, start_y);
 	this->end_node = &m_Model.FindClosestNode(end_x, end_y);
}

float RoutePlanner::CalculateHValue(RouteModel::Node const *node) {
	return node->distance(*(this->end_node));
}

void RoutePlanner::AddNeighbors(RouteModel::Node *current_node) {
    current_node->FindNeighbors();
    for(RouteModel::Node *iN : current_node->neighbors){
      iN->parent = current_node;
      iN->h_value = CalculateHValue(iN);
      iN->g_value = (current_node->g_value) + current_node->distance(*iN);
      iN->visited = true;
      (this->open_list).push_back(iN);
    }
}

bool RoutePlanner::CompareFValue(const RouteModel::Node* node_a, const RouteModel::Node* node_b){
    return (node_a->h_value + node_a->g_value) > (node_b->h_value + node_b->g_value);
}

RouteModel::Node *RoutePlanner::NextNode() {
    std::sort((this->open_list).begin(), (this->open_list).end(), RoutePlanner::CompareFValue);
    RouteModel::Node *lowest = (this->open_list).back();
    (this->open_list).pop_back();
    return lowest;
}

std::vector<RouteModel::Node> RoutePlanner::ConstructFinalPath(RouteModel::Node *current_node) {
    // Create path_found vector
    std::vector<RouteModel::Node> path_found;
    while(current_node != nullptr) {
        if(current_node->parent) distance += current_node->distance(*(current_node->parent));
        path_found.push_back(*current_node);
        current_node = current_node->parent;
    }
        
    distance *= m_Model.MetricScale(); // Multiply the distance by the scale of the map to get meters.
    std::reverse(path_found.begin(), path_found.end());
    return path_found;

}

void RoutePlanner::AStarSearch() {
    RouteModel::Node *current_node = nullptr;
    start_node->visited = true;
    open_list.push_back(start_node);
    
    while(!open_list.empty()) {
        current_node = NextNode();
        if(current_node->distance(*end_node) == 0) {
            std::vector<RouteModel::Node> final_path = ConstructFinalPath(current_node);
            m_Model.path = final_path;
            return;
        }
        AddNeighbors(current_node);
    }
}
