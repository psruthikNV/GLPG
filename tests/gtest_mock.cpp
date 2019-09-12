#include <iostream>
#include <random>

#include "gtest/gtest.h"
#include "math/glpg_math.hpp"
#include <glm/glm.hpp>
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
            glpg::vec3_f generateRandomVec3_f();
            glm::vec3 generateRandomGlmVec3();
            glpg::vec3_f glpgVec[2];
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

    glpg::vec3_f mockTest::generateRandomVec3_f()
    {
        glpg::vec3_f tempVec;

        for (uint32_t i = 0; i < 3; i++) {
            tempVec[i] = dist(gen);
        }
        return tempVec;
    }

    glm::vec3 mockTest::generateRandomGlmVec3() 
    {
        glm::vec3 tempVec;

        for (uint32_t i = 0; i < 3; i++) {
            tempVec[i] = dist(gen);
        }
        return tempVec;
    }

    TEST_F(mockTest, vectorAdditionTest)
    {
        glm::vec3 glm_temp;
        glpg::vec3_f vec_temp;

        glm_temp = glmVec[0] + glmVec[1];
        vec_temp = glpgVec[0] + glpgVec[1];

        for (uint32_t i = 0; i < 3; i++) {
            ASSERT_EQ(glm_temp[i], vec_temp[i]);
        }
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

