//
// Created by Mark Pedersen on 2018-03-01.
//

#ifndef JPS_KMEANS_H
#define JPS_KMEANS_H

#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

struct Point {
private:
    int id_point, id_cluster;
    vector<double> values;
    int total_values;
    string name;

public:
    Point(int id_point, vector<float> &values, string name = "") {
        this->id_point = id_point;
        total_values = values.size();

        for (int i = 0; i < total_values; i++)
            this->values.push_back(values[i]);

        this->name = name;
        id_cluster = -1;
    }

    int getID() {
        return id_point;
    }

    void setCluster(int id_cluster) {
        this->id_cluster = id_cluster;
    }

    int getCluster() {
        return id_cluster;
    }

    double getValue(int index) {
        return values[index];
    }

    int getTotalValues() {
        return total_values;
    }

    void addValue(double value) {
        values.push_back(value);
    }

    string getName() {
        return name;
    }
};

struct Cluster {
private:
    int id_cluster;
    int size;
    vector<double> central_values;
    vector<Point> points;

public:
    Cluster(int id_cluster, Point point) {
        this->id_cluster = id_cluster;

        int total_values = point.getTotalValues();

        for (int i = 0; i < total_values; i++)
            central_values.push_back(point.getValue(i));

        points.push_back(point);
    }

    void addPoint(Point point) {
        points.push_back(point);
    }

    bool removePoint(int id_point) {
        int total_points = points.size();

        for (int i = 0; i < total_points; i++) {
            if (points[i].getID() == id_point) {
                points.erase(points.begin() + i);
                if (size > 0) {
                }
                return true;
            }
        }
        return false;
    }

    double getCentralValue(int index) {
        return central_values[index];
    }

    void setCentralValue(int index, double value) {
        central_values[index] = value;
    }

    Point getPoint(int index) {
        return points[index];
    }

    vector<Point> getPoints() {
        return points;
    }

    int getTotalPoints() {
        return points.size();
    }

    int getID() {
        return id_cluster;
    }
};

class KMeans {
private:
    int K; // number of clusters
    int total_values, total_points, max_iterations;
    vector<Cluster> clusters;

    // return ID of nearest center (uses euclidean distance)
    int getIDNearestCenter(Point point);

public:
    KMeans(int K, int total_points, int total_values, int max_iterations) {
        this->K = K;
        this->total_points = total_points;
        this->total_values = total_values;
        this->max_iterations = max_iterations;
    }

    vector<Cluster> getClusters() {
        return clusters;
    }

    /*!
     * Runs the K-Means algorithm on a bunch of points
     * @param points: the list of points of which clusters will be formed
     */
    void run(vector<Point> &points);
};

#endif //JPS_KMEANS_H