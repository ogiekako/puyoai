#include "evaluation_feature.h"

#include <glog/logging.h>
#include <sstream>
#include <stdio.h>
#include "field.h"

using namespace std;

string EvaluationFeature::toString() const
{
    ostringstream ss;
    ss << get(MAX_CHAINS) << " ";
    ss << (1.0 / get(MAX_RENSA_NECESSARY_PUYOS)) << " ";
    // TODO(mayah): We have to implement this.

    return ss.str();
}

EvaluationParams::EvaluationParams() :
    m_featuresCoef(EvaluationFeature::SIZE_OF_FEATURE_PARAM),
    m_rangeFeaturesCoef(EvaluationFeature::SIZE_OF_RANGE_FEATURE_PARAM)
{
#define DEFINE_PARAM(name) /* ignored */
#define DEFINE_RANGE_PARAM(name, maxValue, asc) m_rangeFeaturesCoef[EvaluationFeature::name].resize(maxValue);
#include "evaluation_feature.tab"
#undef DEFINE_RANGE_PARAM
#undef DEFINE_PARAM

    initialize();
}

void EvaluationParams::initialize()
{
    // これはわりと適当に決めた値

    set(EvaluationFeature::TOTAL_FRAMES, 0.0);
    set(EvaluationFeature::TOTAL_FRAMES_INVERSE, 1.0);

    set(EvaluationFeature::MAX_RENSA_NECESSARY_PUYOS, 0);
    set(EvaluationFeature::MAX_RENSA_NECESSARY_PUYOS_INVERSE, 1.0);

    set(EvaluationFeature::CONNECTION_1, -0.1 / 30.0);
    set(EvaluationFeature::CONNECTION_2,  0.9 / 30.0);
    set(EvaluationFeature::CONNECTION_3,  1.2 / 30.0);
    set(EvaluationFeature::CONNECTION_4, -0.5 / 30.0); // minus
    set(EvaluationFeature::CONNECTION_AFTER_VANISH_1, -0.1 / 15.0);
    set(EvaluationFeature::CONNECTION_AFTER_VANISH_2,  0.9 / 15.0);
    set(EvaluationFeature::CONNECTION_AFTER_VANISH_3,  1.6 / 15.0);
    set(EvaluationFeature::CONNECTION_AFTER_VANISH_4,  0.5 / 15.0); // plus

    set(EvaluationFeature::HAND_WIDTH_1, 0.1);
    set(EvaluationFeature::HAND_WIDTH_2, 0.1);
    set(EvaluationFeature::HAND_WIDTH_3, 0.1);
    set(EvaluationFeature::HAND_WIDTH_4, 0.1);

    set(EvaluationFeature::HAND_WIDTH_RATIO_21, 0.1);
    set(EvaluationFeature::HAND_WIDTH_RATIO_32, 0.1);
    set(EvaluationFeature::HAND_WIDTH_RATIO_43, 0.1);
    set(EvaluationFeature::HAND_WIDTH_RATIO_21_SQUARED, 0.1);
    set(EvaluationFeature::HAND_WIDTH_RATIO_32_SQUARED, 0.1);
    set(EvaluationFeature::HAND_WIDTH_RATIO_43_SQUARED, 0.1);

    set(EvaluationFeature::SUM_OF_HEIGHT_DIFF_FROM_AVERAGE, 0.0);
    set(EvaluationFeature::SQUARE_SUM_OF_HEIGHT_DIFF_FROM_AVERAGE, -0.1);

    set(EvaluationFeature::EMPTY_AVAILABILITY_00, 0.95);
    set(EvaluationFeature::EMPTY_AVAILABILITY_01, 0.90);
    set(EvaluationFeature::EMPTY_AVAILABILITY_02, 0.85);
    set(EvaluationFeature::EMPTY_AVAILABILITY_11, 0.80);
    set(EvaluationFeature::EMPTY_AVAILABILITY_12, 0.75);
    set(EvaluationFeature::EMPTY_AVAILABILITY_22, 0.30);

    for (int i = 0; i < 20; ++i)
        set(EvaluationFeature::MAX_CHAINS, i, i);
    set(EvaluationFeature::THIRD_COLUMN_HEIGHT,  9, -0.5);
    set(EvaluationFeature::THIRD_COLUMN_HEIGHT, 10, -2.0);
    set(EvaluationFeature::THIRD_COLUMN_HEIGHT, 11, -2.0);
    set(EvaluationFeature::THIRD_COLUMN_HEIGHT, 12, -2.0);
    set(EvaluationFeature::THIRD_COLUMN_HEIGHT, 13, -2.0);
    set(EvaluationFeature::THIRD_COLUMN_HEIGHT, 14, -2.0);
    set(EvaluationFeature::THIRD_COLUMN_HEIGHT, 15, -2.0);
}

bool EvaluationParams::save(const char* filename)
{
    FILE* fp = fopen(filename, "w");
    if (!fp) {
        perror("save");
        return false;
    }

    // TODO(mayah): We assume vector memory is continuously allocated.
    fwrite(&m_featuresCoef[0], sizeof(double), m_featuresCoef.size(), fp);
    for (int i = 0; i < EvaluationFeature::SIZE_OF_RANGE_FEATURE_PARAM; ++i)
        fwrite(&m_rangeFeaturesCoef[i][0], sizeof(double), m_rangeFeaturesCoef[i].size(), fp);

    fclose(fp);

    return true;
}

bool EvaluationParams::load(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        perror("load");
        return false;
    }

    // TODO(mayah): We assume vector memory is continuously allocated.
    fread(&m_featuresCoef[0], sizeof(double), m_featuresCoef.size(), fp);
    for (int i = 0; i < EvaluationFeature::SIZE_OF_RANGE_FEATURE_PARAM; ++i)
        fread(&m_rangeFeaturesCoef[i][0], sizeof(double), m_rangeFeaturesCoef[i].size(), fp);

    fclose(fp);
    return true;
}

double EvaluationParams::calculateHandWidthScore(int numFirstCells, int numSecondCells, int numThirdCells, int numFourthCells) const
{
    double d1 = numFirstCells;
    double d2 = numSecondCells;
    double d3 = numThirdCells;
    double d4 = numFourthCells;

    double r21 = d2 / d1;
    double r32 = d3 / d2;
    double r43 = d4 / d3;

    double result = 0;
    result += get(EvaluationFeature::HAND_WIDTH_1) * d1;
    result += get(EvaluationFeature::HAND_WIDTH_2) * d2;
    result += get(EvaluationFeature::HAND_WIDTH_3) * d3;
    result += get(EvaluationFeature::HAND_WIDTH_4) * d4;

    result += get(EvaluationFeature::HAND_WIDTH_RATIO_21) * r21;
    result += get(EvaluationFeature::HAND_WIDTH_RATIO_32) * r32;
    result += get(EvaluationFeature::HAND_WIDTH_RATIO_43) * r43;

    result += get(EvaluationFeature::HAND_WIDTH_RATIO_21_SQUARED) * r21 * r21;
    result += get(EvaluationFeature::HAND_WIDTH_RATIO_32_SQUARED) * r32 * r32;
    result += get(EvaluationFeature::HAND_WIDTH_RATIO_43_SQUARED) * r43 * r43;
    return result;
}

double EvaluationParams::calculateScore(const EvaluationFeature& feature) const
{
    double result = 0;

#define DEFINE_PARAM(name) result += get(EvaluationFeature::name) * feature.get(EvaluationFeature::name);
#define DEFINE_RANGE_PARAM(name, maxValue, asc) result += get(EvaluationFeature::name, feature.get(EvaluationFeature::name));
#include "evaluation_feature.tab"
#undef DEFINE_PARAM
#undef DEFINE_RANGE_PARAM

    LOG(INFO) << "score = " << result;

    return result;
}

string EvaluationParams::toString() const
{
    ostringstream ss;

    // TODO(mayah): We should not use this kind of hack. Use for-loop.
#define DEFINE_PARAM(name) ss << (#name) << " = " << get(EvaluationFeature::name) << endl;
#define DEFINE_RANGE_PARAM(name, maxValue, asc) ss << (#name) << " = "; \
    for (int i = 0; i < (maxValue); ++i) {                              \
        ss << get(EvaluationFeature::name, i) << ' ';                   \
    }                                                                   \
    ss << endl;
#include "evaluation_feature.tab"
#undef DEFINE_PARAM
#undef DEFINE_RANGE_PARAM

    return ss.str();
}
