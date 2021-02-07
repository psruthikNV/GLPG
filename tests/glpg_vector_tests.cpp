#include <iostream>
#include <random>

#include "gtest/gtest.h"
#include "math/GLPGMath.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>

namespace {

    class mockTest : public ::testing::Test {
        private:
            std::random_device rd;
            std::mt19937 gen;
            std::uniform_real_distribution<float> dist;

            void generateTestVertices();
        protected:
            mockTest() {
                gen = std::mt19937(rd());
                dist = std::uniform_real_distribution<float>(-100, 100);
                generateTestVertices();
            }
        public:
            GLPG::vec3_f generateRandomVec3_f();
            glm::vec3 generateRandomGlmVec3();
            GLPG::vec3_f glpgVec[2];
            glm::vec3 glmVec[2];
    };

    void mockTest::generateTestVertices()
    {
        for (uint32_t i = 0; i < 2; i++) {
            for (uint32_t j = 0; j < 3; j++) {
                glmVec[i][j] = glpgVec[i][j] = dist(gen);
            }
        }
    }

    TEST_F(mockTest, vectorAdditionTest)
    {
        glm::vec3 glm_temp;
        GLPG::vec3_f glpg_temp;

        glm_temp = glmVec[0] + glmVec[1];
        glpg_temp = glpgVec[0] + glpgVec[1];

        for (uint32_t i = 0; i < 3; i++) {
            ASSERT_EQ(glm_temp[i], glpg_temp[i]);
        }
    }

    TEST_F(mockTest, vectorSubtractionTest)
    {
        glm::vec3 glm_temp;
        GLPG::vec3_f glpg_temp;

        glm_temp = glmVec[0] - glmVec[1];
        glpg_temp = glpgVec[0] - glpgVec[1];

        for (uint32_t i = 0; i < 3; i++) {
            ASSERT_EQ(glm_temp[i], glpg_temp[i]);
        }
    }

    TEST_F(mockTest, vectorCrossProductTest)
    {
        glm::vec3 glm_temp;
        GLPG::vec3_f glpg_temp;

        glm_temp = glm::cross(glmVec[0], glmVec[1]);
        glpg_temp = glpgVec[0].cross(glpgVec[1]);

        for (uint32_t i = 0; i < 3; i++) {
            ASSERT_EQ(glm_temp[i], glpg_temp[i]);
        }
    }

    TEST_F(mockTest, vectorNormalizeTest)
    {
        glm::vec3 glm_temp;
        GLPG::vec3_f glpg_temp;

        glm_temp = glm::normalize(glmVec[0]);
        glpg_temp = glpgVec[0].normalize();

        for (uint32_t i = 0; i < 3; i++) {
            ASSERT_EQ(glm_temp[i], glpg_temp[i]);
        }
    }

    TEST_F(mockTest, vectorLengthTest)
    {
        float glm_length = glm::length(glmVec[0]);
        float glpg_length = glpgVec[0].length();

        ASSERT_EQ(glm_length, glpg_length);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

