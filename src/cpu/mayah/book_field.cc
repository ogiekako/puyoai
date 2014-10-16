#include "book_field.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>
#include "core/puyo_color.h"

using namespace std;

class PuyoColorEnv {
public:
    PuyoColorEnv()
    {
        for (int i = 0; i < 26; ++i) {
            map_[i] = PuyoColor::WALL;
        }
    }

    PuyoColor map(char var) const
    {
        DCHECK('A' <= var && var <= 'Z') << var;
        return map_[var - 'A'];
    }

    bool isSet(char var) const
    {
        DCHECK('A' <= var && var <= 'Z') << var;
        return map_[var - 'A'] != PuyoColor::WALL;
    }

    PuyoColor set(char var, PuyoColor pc)
    {
        DCHECK('A' <= var && var <= 'Z') << var;
        return map_[var - 'A'] = pc;
    }

private:
    PuyoColor map_[26];
};

static inline
bool check(char currentVar, char neighborVar, PuyoColor neighborColor, const PuyoColorEnv& env)
{
    DCHECK_NE(currentVar, '.');
    DCHECK('A' <= currentVar && currentVar <= 'Z') << currentVar;

    if (neighborColor == PuyoColor::OJAMA || neighborColor == PuyoColor::WALL)
        return true;

    // This case should be already processed.
    if (currentVar == neighborVar)
        return true;

    if (neighborVar == '.') {
        if (env.map(currentVar) == neighborColor) {
            return false;
        }
    } else {
        if (env.map(currentVar) == env.map(neighborVar) && env.isSet(currentVar)) {
            return false;
        }
    }

    return true;
}

BookField::BookField(const string& name, const vector<string>& field, double defaultScore) :
    name_(name),
    defaultScore_(defaultScore)
{
    for (int x = 0; x < PlainField::MAP_WIDTH; ++x) {
        for (int y = 0; y < PlainField::MAP_HEIGHT; ++y) {
            field_[x][y] = '.';
            scoreField_[x][y] = 0;
        }
    }

    for (size_t i = 0; i < field.size(); ++i) {
        CHECK_EQ(field[i].size(), 6U);
        int y = static_cast<int>(field.size()) - i;
        for (int x = 1; x <= 6; ++x) {
           CHECK(('A' <= field[i][x - 1] && field[i][x - 1] <= 'Z') ||
                 ('a' <= field[i][x - 1] && field[i][x - 1] <= 'z') ||
                 (field[i][x - 1] == '.'));

            if ('A' <= field[i][x - 1] && field[i][x - 1] <= 'Z') {
                field_[x][y] = field[i][x - 1];
                scoreField_[x][y] = defaultScore;
            }
        }
    }

    varCount_ = calculateVarCount();
}

void BookField::merge(const BookField& bf)
{
    for (int x = 1; x <= 6; ++x) {
        for (int y = 1; y <= 12; ++y) {
            if (bf.field_[x][y] == '.')
                continue;

            if (field_[x][y] == '.') {
                field_[x][y] = bf.field_[x][y];
                scoreField_[x][y] = bf.scoreField_[x][y];
            } else {
                CHECK_EQ(field_[x][y], bf.field_[x][y]) << name() << " : " << bf.name()
                                                        << "x=" << x << " y=" << y;
                scoreField_[x][y] = bf.scoreField_[x][y];
            }
        }
    }

    varCount_ = calculateVarCount();
}

BookField BookField::mirror() const
{
    BookField bf(*this);
    for (int x = 1; x <= 3; ++x) {
        for (int y = 1; y <= 12; ++y) {
            swap(bf.field_[x][y], bf.field_[7-x][y]);
            swap(bf.scoreField_[x][y], bf.scoreField_[7-x][y]);
        }
    }

    return bf;
}

BookField::MatchResult BookField::match(const PlainField& f) const
{
    // First, make a map from char to PuyoColor.
    int matchCount = 0;
    double matchScore = 0;

    PuyoColorEnv env;

    for (int x = 1; x <= 6; ++x) {
        for (int y = 1; f.get(x, y) != PuyoColor::EMPTY; ++y) {
            char c = field_[x][y];
            if (c == '.')
                continue;

            DCHECK('A' <= c && c <= 'Z') << c;

            PuyoColor pc = f.get(x, y);
            if (pc == PuyoColor::EMPTY)
                continue;

            if (!isNormalColor(pc))
                return MatchResult(false, 0, 0);

            matchCount += 1;
            matchScore += scoreField_[x][y];

            if (!env.isSet(c)) {
                env.set(c, pc);
                continue;
            }

            if (env.map(c) != pc)
                return MatchResult(false, 0, 0);
        }
    }

    // Check the neighbors.
    for (int x = 1; x <= 6; ++x) {
        for (int y = 1; y <= 12 && field_[x][y] != '.'; ++y) {
            if (!check(field_[x][y], field_[x][y + 1], f.get(x, y + 1), env))
                return MatchResult(false, 0, 0);
            if (!check(field_[x][y], field_[x][y - 1], f.get(x, y - 1), env))
                return MatchResult(false, 0, 0);
            if (!check(field_[x][y], field_[x + 1][y], f.get(x + 1, y), env))
                return MatchResult(false, 0, 0);
            if (!check(field_[x][y], field_[x - 1][y], f.get(x - 1, y), env))
                return MatchResult(false, 0, 0);
        }
    }

    return MatchResult(true, matchScore, matchCount);
}

string BookField::toDebugString() const
{
    stringstream ss;
    for (int y = 12; y >= 1; --y) {
        for (int x = 1; x <= 6; ++x) {
            ss << field_[x][y];
        }
        ss << endl;
    }
    return ss.str();
}

int BookField::calculateVarCount() const
{
    int count = 0;
    for (int x = 1; x <= FieldConstant::WIDTH; ++x) {
        for (int y = 1; y <= FieldConstant::HEIGHT; ++y) {
            if ('A' <= field_[x][y] && field_[x][y] <= 'Z') {
                ++count;
            }
        }
    }

    return count;
}
