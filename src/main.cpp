#include <iostream>
#include <memory>
#include <tuple>
#include <vector>
#include <functional>
#include <chrono>
#include <filesystem>


#include <Eigen/Dense>

#include <drake/planning/robot_diagram_builder.h>
#include <drake/multibody/parsing/parser.h>
#include <drake/visualization/visualization_config_functions.h>
#include <drake/planning/scene_graph_collision_checker.h>
#include <drake/planning/collision_checker_params.h>
#include <drake/planning/iris/iris_from_clique_cover.h>
#include <drake/common/random.h>
#include <drake/geometry/optimization/hpolyhedron.h>

#include "visualization.hpp"
#include "planar_arm.hpp"
#include "generalized_bur.hpp"
#include "gbur_iris.hpp"
#include "testing.hpp"
#include "anthropomorphic_arm.hpp"


class DrakeRandomGenerator {
public:
    DrakeRandomGenerator(
        const drake::geometry::optimization::HPolyhedron& domain,
        drake::RandomGenerator& randomGenerator
    ) : domain{ domain },
        randomGenerator{ randomGenerator },
        lastSample{ std::nullopt } {}

    Eigen::VectorXd randomConfig() {
        if (!lastSample) {
            lastSample = domain.UniformSample(&randomGenerator);
        } else {
            lastSample = domain.UniformSample(&randomGenerator, *lastSample);
        }

        return *lastSample;
    }

private:
    const drake::geometry::optimization::HPolyhedron& domain;
    drake::RandomGenerator& randomGenerator;
    std::optional<Eigen::VectorXd> lastSample;

};




// int main() {
//     constexpr int n{ 3 };
//
//     auto mat{ GBurIRIS::generateRandomRotationMatrix(n) };
//
//
//     Eigen::Vector3d p1{mat * Eigen::Vector3d(1, 0, 0)},
//         p2{mat * Eigen::Vector3d(0, 1, 0)},
//         p3{mat * Eigen::Vector3d(0, 0, 1)};
//
//
//     std::cout << p1.transpose() << std::endl
//               << p2.transpose() << std::endl
//               << p3.transpose() << std::endl
//               << p1.dot(p2) << std::endl
//               << p1.dot(p3) << std::endl
//               << p2.dot(p3) << std::endl;
//
//
// //     matplotlibcpp::plot({0, 0}, {0, 1}, {{"color", "red"}});
// //     matplotlibcpp::plot({0}, {1}, {{"color", "red"}, {"marker", "."}});
// //     matplotlibcpp::plot({0, 1}, {0, 0}, {{"color", "red"}});
// //     matplotlibcpp::plot({1}, {0}, {{"color", "red"}, {"marker", "."}});
// //
// //     matplotlibcpp::plot({0, p1(0)}, {0, p1(1)}, {{"color", "blue"}});
// //     matplotlibcpp::plot({p1(0)}, {p1(1)}, {{"color", "blue"}, {"marker", "."}});
// //     matplotlibcpp::plot({0, p2(0)}, {0, p2(1)}, {{"color", "blue"}});
// //     matplotlibcpp::plot({p2(0)}, {p2(1)}, {{"color", "blue"}, {"marker", "."}});
// //
// //     matplotlibcpp::show();
//
//
//
//     return 0;
// }

/*
    ([0, 0, 0], np.array([1.7, 1.7, 0.9])),
    ([0, np.pi/2, 0], np.array([0.1, 1.7, 0.9])),*/

// int main__() {
//     drake::planning::RobotDiagramBuilder<double> robotDiagramBuilder;
//     drake::multibody::MultibodyPlant<double>& plant{ robotDiagramBuilder.plant() };
//
//     std::string projectPath{ std::filesystem::current_path().parent_path().string() };
//
//     drake::multibody::Parser parser(&plant);
//     parser.package_map().Add("assets", projectPath + "/assets");
//     parser.AddModels(projectPath + "/scenes/3dofScene1.dmd.yaml");
//     plant.Finalize();
//
//     std::unique_ptr<drake::planning::RobotDiagram<double>> diagram{ robotDiagramBuilder.Build() };
//     std::unique_ptr<drake::systems::Context<double>> diagramContext{ diagram->CreateDefaultContext() };
//
//     drake::planning::CollisionCheckerParams collisionCheckerParams;
//     collisionCheckerParams.env_collision_padding =
//         drake::geometry::optimization::IrisOptions().configuration_space_margin;
//     collisionCheckerParams.model = std::move(diagram);
//     collisionCheckerParams.edge_step_size = 0.1;
//     collisionCheckerParams.robot_model_instances.push_back(plant.GetModelInstanceByName("AnthropomorphicArm"));
//
//     std::unique_ptr<drake::planning::CollisionChecker> collisionChecker{
//         std::make_unique<drake::planning::SceneGraphCollisionChecker>(std::move(collisionCheckerParams))
//     };
//
//     std::vector<std::reference_wrapper<const drake::multibody::RigidBody<double>>> jointChildAndEndEffectorLinks {
//         plant.GetBodyByName("AnthropomorphicArmLink1", plant.GetModelInstanceByName("AnthropomorphicArm")),
//         plant.GetBodyByName("AnthropomorphicArmLink2", plant.GetModelInstanceByName("AnthropomorphicArm")),
//         plant.GetBodyByName("AnthropomorphicArmLink3", plant.GetModelInstanceByName("AnthropomorphicArm")),
//         plant.GetBodyByName("AnthropomorphicArmEndEffector", plant.GetModelInstanceByName("AnthropomorphicArm"))
//     };
//
//     std::vector<double> linkGeometryCompensation(3, 0.1);
//
//     GBurIRIS::robots::AnthropomorphicArm anthropomorphicArm(*collisionChecker, jointChildAndEndEffectorLinks, linkGeometryCompensation);
//
//     for (auto r : anthropomorphicArm.getEnclosingRadii(Eigen::Vector3d(0, 0, 0))) {
//         std::cout << r << std::endl;
//     }
//
// }


int main() {
    drake::planning::RobotDiagramBuilder<double> robotDiagramBuilder;
    drake::multibody::MultibodyPlant<double>& plant{ robotDiagramBuilder.plant() };

    std::string projectPath{ std::filesystem::current_path().parent_path().string() };

    drake::multibody::Parser parser(&plant);
    parser.package_map().Add("assets", projectPath + "/assets");
    parser.AddModels(projectPath + "/scenes/3dofScene3.dmd.yaml");
    plant.Finalize();

    std::unique_ptr<drake::planning::RobotDiagram<double>> diagram{ robotDiagramBuilder.Build() };
    std::unique_ptr<drake::systems::Context<double>> diagramContext{ diagram->CreateDefaultContext() };

    drake::planning::CollisionCheckerParams collisionCheckerParams;
    collisionCheckerParams.model = std::move(diagram);
    collisionCheckerParams.edge_step_size = 0.1;
//     collisionCheckerParams.robot_model_instances.push_back(plant.GetModelInstanceByName("6dofPlanarArm"));
    collisionCheckerParams.robot_model_instances.push_back(plant.GetModelInstanceByName("AnthropomorphicArm"));
//     collisionCheckerParams.robot_model_instances.push_back(plant.GetModelInstanceByName("2dofPlanarArm"));

    std::unique_ptr<drake::planning::CollisionChecker> collisionChecker{
        std::make_unique<drake::planning::SceneGraphCollisionChecker>(std::move(collisionCheckerParams))
    };

    auto&& bodyIndices{ plant.GetBodyIndices(plant.GetModelInstanceByName("AnthropomorphicArm")) };
    for (int i{}; i < bodyIndices.size(); ++i) {
        for (int j{i + 1}; j < bodyIndices.size(); ++j) {
            collisionChecker->SetCollisionFilteredBetween(bodyIndices.at(i), bodyIndices.at(j), true);
        }
    }

//     std::vector<std::reference_wrapper<const drake::multibody::RigidBody<double>>> jointChildAndEndEffectorLinks {
//         plant.GetBodyByName("6dofPlanarLink1", plant.GetModelInstanceByName("6dofPlanarArm")),
//         plant.GetBodyByName("6dofPlanarLink2", plant.GetModelInstanceByName("6dofPlanarArm")),
//         plant.GetBodyByName("6dofPlanarLink3", plant.GetModelInstanceByName("6dofPlanarArm")),
//         plant.GetBodyByName("6dofPlanarLink4", plant.GetModelInstanceByName("6dofPlanarArm")),
//         plant.GetBodyByName("6dofPlanarLink5", plant.GetModelInstanceByName("6dofPlanarArm")),
//         plant.GetBodyByName("6dofPlanarLink6", plant.GetModelInstanceByName("6dofPlanarArm")),
//         plant.GetBodyByName("6dofPlanarEndEffector", plant.GetModelInstanceByName("6dofPlanarArm"))
//     };

    std::vector<std::reference_wrapper<const drake::multibody::RigidBody<double>>> jointChildAndEndEffectorLinks {
        plant.GetBodyByName("AnthropomorphicArmLink1", plant.GetModelInstanceByName("AnthropomorphicArm")),
        plant.GetBodyByName("AnthropomorphicArmLink2", plant.GetModelInstanceByName("AnthropomorphicArm")),
        plant.GetBodyByName("AnthropomorphicArmLink3", plant.GetModelInstanceByName("AnthropomorphicArm")),
        plant.GetBodyByName("AnthropomorphicArmEndEffector", plant.GetModelInstanceByName("AnthropomorphicArm"))
    };

//     std::vector<std::reference_wrapper<const drake::multibody::RigidBody<double>>> jointChildAndEndEffectorLinks {
//         plant.GetBodyByName("2dofPlanarLink1", plant.GetModelInstanceByName("2dofPlanarArm")),
//         plant.GetBodyByName("2dofPlanarLink2", plant.GetModelInstanceByName("2dofPlanarArm")),
//         plant.GetBodyByName("2dofPlanarEndEffector", plant.GetModelInstanceByName("2dofPlanarArm"))
//     };


//     std::vector<double> linkGeometryCompensation(6, 0.1);
    std::vector<double> linkGeometryCompensation(3, 0.1);
//     std::vector<double> linkGeometryCompensation(2, 0.1);

    GBurIRIS::robots::AnthropomorphicArm anthropomorphicArm(
        *collisionChecker,
        jointChildAndEndEffectorLinks,
        linkGeometryCompensation
    );

//     GBurIRIS::robots::PlanarArm planarArm(*collisionChecker, jointChildAndEndEffectorLinks, linkGeometryCompensation);




    int numOfRuns{ 10 };

    GBurIRIS::GBurIRISConfig gBurIRISConfig;
    gBurIRISConfig.coverage = 0.7;
    gBurIRISConfig.numOfSpines = 6;

    GBurIRIS::testing::TestGBurIRIS testGBurIRIS(anthropomorphicArm, gBurIRISConfig);
    auto [execTimeGBurIRIS, numOfRegionsGBurIRIS, coverageGBurIRIS] = testGBurIRIS.run(numOfRuns);

    GBurIRIS::testing::TestGBurIRIS testGBurIRIS2(
        anthropomorphicArm,
        gBurIRISConfig,
        GBurIRIS::testing::TestGBurIRIS::GBurDistantConfigOption::rotationMatrix
    );
    auto [execTimeGBurIRIS2, numOfRegionsGBurIRIS2, coverageGBurIRIS2] = testGBurIRIS2.run(numOfRuns);

    drake::planning::IrisFromCliqueCoverOptions irisFromCliqueCoverOptions;
    irisFromCliqueCoverOptions.coverage_termination_threshold = gBurIRISConfig.coverage;
    irisFromCliqueCoverOptions.num_points_per_visibility_round = 500;
    irisFromCliqueCoverOptions.num_points_per_coverage_check = 5000;
    irisFromCliqueCoverOptions.minimum_clique_size = 10;


    collisionChecker->SetPaddingAllRobotEnvironmentPairs(irisFromCliqueCoverOptions.iris_options.configuration_space_margin);

    GBurIRIS::testing::TestVCC testVCC(anthropomorphicArm, irisFromCliqueCoverOptions);
    auto [execTimeVCC, numOfRegionsVCC, coverageVCC] = testVCC.run(numOfRuns);


    std::cout << testGBurIRIS.calculateMean(execTimeGBurIRIS) << "+-"
                << testGBurIRIS.calculateStandardDeviation(execTimeGBurIRIS) << " "
              << testGBurIRIS.calculateMean(numOfRegionsGBurIRIS) << "+-"
                << testGBurIRIS.calculateStandardDeviation(numOfRegionsGBurIRIS) << " "
              << testGBurIRIS.calculateMean(coverageGBurIRIS) << "+-"
                << testGBurIRIS.calculateStandardDeviation(coverageGBurIRIS) << std::endl
              << testGBurIRIS2.calculateMean(execTimeGBurIRIS2) << "+-"
                << testGBurIRIS2.calculateStandardDeviation(execTimeGBurIRIS2) << " "
              << testGBurIRIS2.calculateMean(numOfRegionsGBurIRIS2) << "+-"
                << testGBurIRIS2.calculateStandardDeviation(numOfRegionsGBurIRIS2) << " "
              << testGBurIRIS2.calculateMean(coverageGBurIRIS2) << "+-"
                << testGBurIRIS2.calculateStandardDeviation(coverageGBurIRIS2) << std::endl
              << testVCC.calculateMean(execTimeVCC) << "+-"
                << testVCC.calculateStandardDeviation(execTimeVCC) << " "
              << testVCC.calculateMean(numOfRegionsVCC)<< "+-"
                << testVCC.calculateStandardDeviation(numOfRegionsVCC) << " "
              << testVCC.calculateMean(coverageVCC)<< "+-"
                << testVCC.calculateStandardDeviation(coverageVCC) << std::endl;

    return 0;
}


// int main_() {
//     drake::planning::RobotDiagramBuilder<double> robotDiagramBuilder;
//     drake::multibody::MultibodyPlant<double>& plant{ robotDiagramBuilder.plant() };
//
//     std::string projectPath{ std::filesystem::current_path().parent_path().string() };
//
//     drake::multibody::Parser parser(&plant);
//     parser.package_map().Add("assets", projectPath + "/assets");
//     parser.AddModels(projectPath + "/scenes/2dofScene3.dmd.yaml");
//     plant.Finalize();
//
//     // drake::visualization::AddDefaultVisualization(&robotDiagramBuilder.builder());
//
//     std::unique_ptr<drake::planning::RobotDiagram<double>> diagram{ robotDiagramBuilder.Build() };
//     std::unique_ptr<drake::systems::Context<double>> diagramContext{ diagram->CreateDefaultContext() };
//     // drake::systems::Context<double>& plantContext{ plant.GetMyMutableContextFromRoot(diagramContext.get()) };
//
//     // diagram->ForcedPublish(*diagramContext);
//
//     drake::planning::CollisionCheckerParams collisionCheckerParams;
//     collisionCheckerParams.model = std::move(diagram);
//     collisionCheckerParams.edge_step_size = 0.1;
//     collisionCheckerParams.robot_model_instances.push_back(plant.GetModelInstanceByName("2dofPlanarArm"));
//
//     std::unique_ptr<drake::planning::CollisionChecker> collisionChecker{
//         std::make_unique<drake::planning::SceneGraphCollisionChecker>(std::move(collisionCheckerParams))
//     };
//
//     std::vector<std::reference_wrapper<const drake::multibody::RigidBody<double>>> jointChildAndEndEffectorLinks {
//         plant.GetBodyByName("2dofPlanarLink1", plant.GetModelInstanceByName("2dofPlanarArm")),
//         plant.GetBodyByName("2dofPlanarLink2", plant.GetModelInstanceByName("2dofPlanarArm")),
//         plant.GetBodyByName("2dofPlanarEndEffector", plant.GetModelInstanceByName("2dofPlanarArm"))
//     };
//
//     std::vector<double> linkGeometryCompensation{ 0.1, 0.1 };
//
//     GBurIRIS::robots::PlanarArm planarArm(*collisionChecker, jointChildAndEndEffectorLinks, linkGeometryCompensation);
//
// //     auto&& config1{ Eigen::Vector2d(0, 3.14) }, config2{ Eigen::Vector2d(0, -1.57) };
// //
// //     for (auto&& pos : planarArm.getLinkPositions(config1)) {
// //         std::cout << pos << std::endl;
// //     }
// //
// //     for (auto&& radius : planarArm.getEnclosingRadii(config1)) {
// //         std::cout << radius << " ";
// //     }
// //     std::cout << std::endl;
// //
// //     std::cout << planarArm.getMaxDisplacement(config1, config2) << std::endl;
//
//
// //     drake::RandomGenerator randomGenerator(0);
// //     auto domain = drake::geometry::optimization::HPolyhedron::MakeBox(
// //         plant.GetPositionLowerLimits(),
// //         plant.GetPositionUpperLimits()
// //     );
// //     DrakeRandomGenerator drakeRandomGenerator(domain, randomGenerator);
// //
// //     GBurIRIS::GBurIRISConfig gBurIRISConfig;
// //     gBurIRISConfig.numOfIter = 1;
// //     gBurIRISConfig.coverage = 0.9;
// //
// //     auto [regionsGBurIRIS, coverageGBurIRIS, burs] = GBurIRIS::GBurIRIS2(
// //         planarArm,
// //         gBurIRISConfig,
// //         std::bind(&DrakeRandomGenerator::randomConfig, &drakeRandomGenerator)
// //     );
// //
// //
// //     int numOfSamples{ 250 };
// //     GBurIRIS::visualization::Figure figure;
// //     figure.visualize2dConfigurationSpace(*collisionChecker, numOfSamples);
// //
// //     figure.visualize2dGeneralizedBur(
// //         *collisionChecker,
// //         burs.at(0),
// //         numOfSamples
// //     );
// //
// //     figure.showFigures();
//
//
//     int numOfRuns{ 5 };
//
//     GBurIRIS::GBurIRISConfig gBurIRISConfig;
//     gBurIRISConfig.coverage = 0.9;
//
//     GBurIRIS::testing::TestGBurIRIS testGBurIRIS(planarArm, gBurIRISConfig);
//     auto [execTimeGBurIRIS, numOfRegionsGBurIRIS, coverageGBurIRIS] = testGBurIRIS.run(numOfRuns);
//
//     gBurIRISConfig.numOfSpines = 4;
//     GBurIRIS::testing::TestGBurIRIS testGBurIRIS2(
//         planarArm,
//         gBurIRISConfig,
//         GBurIRIS::testing::TestGBurIRIS::GBurDistantConfigOption::rotationMatrix
//     );
//     auto [execTimeGBurIRIS2, numOfRegionsGBurIRIS2, coverageGBurIRIS2] = testGBurIRIS2.run(numOfRuns);
//
//     drake::planning::IrisFromCliqueCoverOptions irisFromCliqueCoverOptions;
//     irisFromCliqueCoverOptions.coverage_termination_threshold = 0.9;
//     irisFromCliqueCoverOptions.num_points_per_visibility_round = 500;
//     irisFromCliqueCoverOptions.num_points_per_coverage_check = 5000;
//     irisFromCliqueCoverOptions.minimum_clique_size = 10;
//
//
//     GBurIRIS::testing::TestVCC testVCC(planarArm, irisFromCliqueCoverOptions);
//     auto [execTimeVCC, numOfRegionsVCC, coverageVCC] = testVCC.run(numOfRuns);
//
//
//     std::cout << testGBurIRIS.calculateMean(execTimeGBurIRIS) << " "
//               << testGBurIRIS.calculateMean(numOfRegionsGBurIRIS) << " "
//               << testGBurIRIS.calculateMean(coverageGBurIRIS) << std::endl
//               << testGBurIRIS.calculateMean(execTimeGBurIRIS2) << " "
//               << testGBurIRIS.calculateMean(numOfRegionsGBurIRIS2) << " "
//               << testGBurIRIS.calculateMean(coverageGBurIRIS2) << std::endl
//               << testVCC.calculateMean(execTimeVCC) << " "
//               << testVCC.calculateMean(numOfRegionsVCC) << " "
//               << testVCC.calculateMean(coverageVCC) << std::endl;
//
//
//
// /*
//     drake::RandomGenerator randomGenerator(0);
//     auto domain = drake::geometry::optimization::HPolyhedron::MakeBox(
//         plant.GetPositionLowerLimits(),
//         plant.GetPositionUpperLimits()
//     );
//     DrakeRandomGenerator drakeRandomGenerator(domain, randomGenerator);
//
//     GBurIRIS::GBur::GeneralizedBur gBur(
//         Eigen::Vector2d(0, 0),
//         GBurIRIS::GBur::GeneralizedBurConfig{ 20, 2, 1e-5, 0.01 },
//         planarArm,
//         std::bind(&DrakeRandomGenerator::randomConfig, &drakeRandomGenerator)
//     );
//
//
//     std::vector<Eigen::VectorXd> randomConfigs {
//         Eigen::Vector2d(2.41516081, -12.3320883),
//         Eigen::Vector2d(-2.68751996, 12.2756116),
//         Eigen::Vector2d(-9.98523574, -7.62944761),
//         Eigen::Vector2d(5.83260261, 11.13077492),
//         Eigen::Vector2d(-12.30621474,   2.54371429),
//         Eigen::Vector2d(10.08631485, -7.49530897),
//         Eigen::Vector2d(-7.87107069, -9.79589964),
//         Eigen::Vector2d(-6.35049916, 10.84364164),
//         Eigen::Vector2d(8.65546941, -9.11022793),
//         Eigen::Vector2d(10.22488112, -7.30514954),
//         Eigen::Vector2d(-1.92731445, 12.41768347),
//         Eigen::Vector2d(11.16758691, -5.76180582),
//         Eigen::Vector2d(-11.70043926,  -4.58400751),
//         Eigen::Vector2d(-11.23589189,  -5.62744498),
//         Eigen::Vector2d(6.5435321 , -10.72826133),
//         Eigen::Vector2d(8.66562355, 9.10056988),
//         Eigen::Vector2d(-12.0397344 ,  -3.59974987),
//         Eigen::Vector2d(-9.61137696,  8.09535895),
//         Eigen::Vector2d(9.27088909, -8.48316091),
//         Eigen::Vector2d(12.32960736,  2.42779449)
//     };
//
//     gBur.setRandomConfigs(randomConfigs);
//
//     auto [randomConfigs_, layers] = gBur.calculateBur();
//
//
//     std::vector<Eigen::VectorXd> outerLayer;
//     for (auto&& spine : layers) {
//         outerLayer.push_back(*(spine.end() - 1));
//     }
//
//
//     auto ellipsoid{ GBurIRIS::MinVolumeEllipsoid(*collisionChecker, outerLayer) };
//
//     auto polytope{ GBurIRIS::InflatePolytope(*collisionChecker, ellipsoid) };
//
//     int numOfSamples{ 250 };
//     GBurIRIS::visualization::Figure figure;
//     figure.visualize2dConfigurationSpace(*collisionChecker, numOfSamples);
//
//     figure.visualize2dConvexSet(
//         *collisionChecker,
//         polytope,
//         numOfSamples,
//         std::make_tuple(0, 1, 1, 1)
//     );
//
//     figure.visualize2dConvexSet(
//         *collisionChecker,
//         ellipsoid,
//         numOfSamples,e
//         std::make_tuple(0.5, 0.5, 0.5, 1)
//     );
//
//
//     figure.visualize2dGeneralizedBur(
//         *collisionChecker,
//         gBur,
//         numOfSamples,
//         std::vector<std::tuple<double, double, double, double>>{
//             {0.75, 0, 0, 1},
//             {0, 0.75, 0, 1},
//             {0, 0, 0.75, 1}
//         }
//     );
//
//     figure.showFigures();*/
//
//
//
// //     robots::Robot robot(*collisionChecker, std::vector<std::shared>);
//
//
// //     drake::planning::RobotDiagramBuilder<double> robotDiagramBuilder;
// //     drake::multibody::MultibodyPlant<double>& plant{ robotDiagramBuilder.plant() };
// //
// //     drake::multibody::Parser parser(&plant);
// //     parser.package_map().Add("assets", "/home/dzenan/Desktop/TestDrake/assets");
// //     parser.AddModels("/home/dzenan/Desktop/TestDrake/scenes/2dofScene0.dmd.yaml");
// //     plant.Finalize();
// //
// //     // drake::visualization::AddDefaultVisualization(&robotDiagramBuilder.builder());
// //
// //     std::unique_ptr<drake::planning::RobotDiagram<double>> diagram{ robotDiagramBuilder.Build() };
// //     std::unique_ptr<drake::systems::Context<double>> diagramContext{ diagram->CreateDefaultContext() };
// //     // drake::systems::Context<double>& plantContext{ plant.GetMyMutableContextFromRoot(diagramContext.get()) };
// //
// //     // diagram->ForcedPublish(*diagramContext);
// //
// //     drake::planning::CollisionCheckerParams collisionCheckerParams;
// //     collisionCheckerParams.model = std::move(diagram);
// //     collisionCheckerParams.edge_step_size = 0.1;
// //     collisionCheckerParams.robot_model_instances.push_back(plant.GetModelInstanceByName("2dofPlanarArm"));
// //
// //     std::unique_ptr<drake::planning::CollisionChecker> collisionChecker{
// //         new drake::planning::SceneGraphCollisionChecker(std::move(collisionCheckerParams))
// //     };
// //
// //     drake::planning::IrisFromCliqueCoverOptions irisFromCliqueCoverOptions;
// //     irisFromCliqueCoverOptions.coverage_termination_threshold = 0.8;
// //     irisFromCliqueCoverOptions.num_points_per_visibility_round = 500;
// //     irisFromCliqueCoverOptions.num_points_per_coverage_check = 5000;
// //     irisFromCliqueCoverOptions.minimum_clique_size = 10;
// //
// //     drake::RandomGenerator randomGenerator(1337);
// //     std::vector<drake::geometry::optimization::HPolyhedron> sets;
// //
// //     drake::planning::IrisInConfigurationSpaceFromCliqueCover(
// //         *collisionChecker,
// //         irisFromCliqueCoverOptions,
// //         &randomGenerator,
// //         &sets
// //     );
// //
// //     std::srand(0);
// //     visualization::Figure figure;
// //     figure.visualize2dConfigurationSpace(*collisionChecker, 250);
// //     for (auto&& set : sets) {
// //         figure.visualize2dConvexSet(collisionChecker->plant(), set, 250);
// //     }
// //
// //     visualization::Figure::showFigures();
//
//
//     return 0;
// }
