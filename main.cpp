#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

# define GREEN_BOLD "\e[1;32m"
# define RED_BOLD "\e[1;31m"
# define UNSET "\033[0m"

#include <sys/stat.h>
#include <errno.h>


// From
// https://stackoverflow.com/questions/675039/
static bool isDirExist(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

static bool makePath(const std::string& path) {
    mode_t mode = 0755;
    int ret = mkdir(path.c_str(), mode);
    if (ret == 0)
        return true;

    switch (errno) {
    case ENOENT:
        // parent didn't exist, try to create it
        {
            size_t pos = path.find_last_of('/');
            // case path ends with /
            if (path.length() != 0 && pos == path.length() - 1)
                pos = (path.substr(0, pos)).find_last_of('/');
            if (pos == std::string::npos)
                return false;
            // recursive call with shorter path:
            // path = A/B/C/D -> A/B/C -> A/B -> A
            if (!makePath( path.substr(0, pos) ))
                return false;
        }
        // now, try to create again
        return 0 == mkdir(path.c_str(), mode);
    case EEXIST:
        // done!
        return isDirExist(path);
    default:
        return false;
    }
}

class Team {

    public:

    Team(std::string team_name)
    :
        name(team_name),
        wins(0),
        losses(0),
        draws(0)
    {}

    ~Team() {}

    void reset() {
        wins = 0;
        losses = 0;
        draws = 0;
    }

    int getPoints() {
        return (3*wins) + (draws);
    }

    std::string name;
    int wins;
    int losses;
    int draws;
};

typedef enum {
    LOSS = 0,
    DRAW,
    WIN
} RESULTADO;

// result is A vs B
void match( Team& A, Team& B, int resultado) {

    if (resultado == WIN) {
        A.wins += 1;
        B.losses += 1;
    } else if (resultado == LOSS) {
        A.losses += 1;
        B.wins += 1;
    } else {
        A.draws += 1;
        B.draws += 1;
    }
}

// Possible combinations of 6 match results with repetition and order.
void fill_possible_results(std::vector<std::vector<int>>& possible_results) {

    for (int i1 = 0; i1 < 3; i1++) {
        for (int i2 = 0; i2 < 3; i2++) {
            for (int i3 = 0; i3 < 3; i3++) {
                for (int i4 = 0; i4 < 3; i4++) {
                    for (int i5 = 0; i5 < 3; i5++) {
                        for (int i6 = 0; i6 < 3; i6++) {
                            std::vector<int> result;
                            result.push_back(i1);
                            result.push_back(i2);
                            result.push_back(i3);
                            result.push_back(i4);
                            result.push_back(i5);
                            result.push_back(i6);
                            possible_results.push_back(result);
                        }
                    }
                }
            }
        }
    }
}

void print_result(std::vector<Team> group, std::vector<int> result) {

    std::vector<int> points;
    for (int i = 0; i < 4; i++) {
        points.push_back(group[i].getPoints());
    }
    std::sort(points.begin(), points.end(), std::greater<int>());

    for (int i = 0; i < 4; i++) {
        std::cout << group[i].name;
        if (group[i].getPoints() >= *(points.begin() + 1)) {
            std::cout << GREEN_BOLD;
        } else {
            std::cout << RED_BOLD;
        }
        std::cout << "  W : " << group[i].wins
                  << ", L : " << group[i].losses
                  << ", D : " << group[i].draws
                  << ", points: " << group[i].getPoints()
                  << UNSET << std::endl; 
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

int main(int argc, char *argv) {

    Team ESP("Team A");
    Team GER("Team B");
    Team ARG("Team C");
    Team POR("Team D");
    std::vector<Team> group = {ESP, GER, ARG, POR};
    std::vector<std::vector<int>> possible_results;

    fill_possible_results(possible_results);

    for (auto result : possible_results) {
        int match_index = 0;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < i; j++) {
                match(group[i], group[j], result[match_index++]);
            }
        }
        print_result(group, result);
        for (int i = 0; i < 4; i++) {
            group[i].reset();
        }
    }
	return 0;
}
