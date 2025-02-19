#include <iostream>
#include "../basic/linalg.h"
#include "../ml/kmeans.h"
#include "../ml/svm.h"
#include "../ml/gmm.h"
#include "../ml/hmm.h"
#include "../ml/kdtree.hpp"
#include "../utils/dataset.h"
#include "../ml/hopfieldnet.hpp"

void test_kmeans()
{
    /* load data */
    NumericDB db("D:/home/dataset/wine-clustering.csv");
    std::vector<Tensor> x;
    db.load(x);
    /* clustering */
    Kmeans model(3, 10);
    model.cluster(x, 1000);
    /* predict */
    std::size_t label = model(x[0]);
    std::cout<<"label:"<<label<<std::endl;
#if 0
    /* project to 2d-plane */
    LinearAlgebra::PCA pca;
    Mat x1(x.size(), x[0].shape[1]);

    Mat::fromArray(x, x1);
    pca.fit(x1);
    Tensor y;
    pca.project(x[0], 2, y);
    y.printValue();
#endif
    return;
}

void test_kdtree()
{
    std::vector<Tensor> x = {Tensor({4, 1}, {1, 1, 2, 3}),
                             Tensor({4, 1}, {0, 5, 4, 0}),
                             Tensor({4, 1}, {1, -1, 0, 1}),
                             Tensor({4, 1}, {2, 1, 3, -5}),
                             Tensor({4, 1}, {-1, 0, 1, 1}),
                             Tensor({4, 1}, {9, 2, 7, 8}),
                             Tensor({4, 1}, {7, 9, -1, 2}),
                             Tensor({4, 1}, {2, -4, 9, 0}),
                             Tensor({4, 1}, {4, 1, 0, 3}),
                             Tensor({4, 1}, {8, 2, 1, 1}),
                             Tensor({4, 1}, {0, 1, 1, 1}),
                             Tensor({4, 1}, {0, 0, 0, 1})};

    Tensor u(1, 4);
    LinAlg::mean(x, u);
    u.printValue();
    Tensor sigma(1, 4);
    LinAlg::variance(x, u, sigma);
    sigma.printValue();
    //return 0;
    KDTree kdtree(x);
    //kdtree.display();
    //return 0;
    std::vector<KDTree::Result> results;
    Tensor xi({4, 1}, {0, 0, 0, 0});
    kdtree.find(xi, results);
    for (std::size_t i = 0; i < results.size(); i++) {
        results[i].value.printValue();
        std::cout<<"d = "<<results[i].distance<<std::endl;
    }
    return;
}

void test_svm()
{
    Tensor x({4, 2, 1}, {1, 1,
                         1, 0,
                         0, 1,
                         0, 0});
    Tensor y({4, 1}, {-1, 1, 1, -1});
    std::vector<Tensor> xi;
    x.toVector(xi);
    SVM svm([](const Tensor &x1, const Tensor &x2)->float{
        return LinAlg::Kernel::polynomial(x1, x2, 1, 10);
    }, 1e-4, 1);
    svm.fit(xi, y, 1000);
    /* predict */
    Tensor p(4, 1);
    for (int i = 0; i < 4; i++) {
        float s = svm(xi[i]);
        if (s > 0) {
            p[i] = 1;
        } else {
            p[i] = 0;
        }
        std::cout<<s<<std::endl;
    }
    p.printValue();
    return;
}

void test_gaussian()
{
    Tensor x({3, 3, 1}, {2, 4, 10,
                         3, 1, 2,
                         7, 10, 5});
    Tensor u({3, 1}, {4, 5, 6});
    Tensor s({3, 1}, {13/3, 14, 14});
    std::vector<Tensor> x_;
    x.toVector(x_);
    for (int i = 0; i < 3; i++) {
        float p = GMM::gaussian(x_[i], u, LinAlg::diag(s));
        std::cout<<p<<", ";
    }
    std::cout<<std::endl;
    for (int i = 0; i < 3; i++) {
        float p = GMM::quickGaussian(x_[i], u, s);
        std::cout<<p<<", ";
    }
    std::cout<<std::endl;
    return;
}

void test_hmm()
{
    Tensor A({4, 4}, {0,1,0,0,
                      0.4,0,0.6,0,
                      0,0.4,0,0.6,
                      0,0,0.5,0.5});
    Tensor B({4, 2}, {0.5,0.5,
                      0.3,0.7,
                      0.6,0.4,
                      0.8,0.2});
    Tensor Pi({4, 1}, {0.25,0.25,0.25,0.25});
    Tensor O({8, 1}, {1, 0, 0, 1, 1, 0, 1, 1});
    HMM model(A, B, Pi);
    Tensor I = model(O);
    I.printValue();
    return;
}

void test_hopfieldNet()
{
    /* T */
    Tensor x1({25, 1}, {
                   1,  1,  1,  1,  1,
                  -1, -1,  1, -1, -1,
                  -1, -1,  1, -1, -1,
                  -1, -1,  1, -1, -1,
                  -1, -1,  1, -1, -1
              });
    /* H */
    Tensor x2({25, 1}, {
                   1,  1, -1,  1,  1,
                   1,  1, -1,  1,  1,
                   1,  1,  1,  1,  1,
                   1,  1, -1,  1,  1,
                   1,  1, -1,  1,  1
              });
    /* X */
    Tensor x3({25, 1}, {
                   1, -1, -1, -1,  1,
                  -1,  1, -1,  1, -1,
                  -1, -1,  1, -1, -1,
                  -1,  1, -1,  1, -1,
                   1, -1, -1, -1,  1
              });

    std::vector<Tensor> x = {x1, x2, x3};
    /* model */
    HopfieldNet model(25, 0.01);
    model.fit(x);
    /* predict: restore origin data from noised data */
    std::cout<<"T:"<<std::endl;
    Tensor xt({25, 1}, {
                  -1, -1,  1,  1,  1,
                  -1, -1,  1, -1, -1,
                  -1, -1,  1, -1, -1,
                  -1, -1,  1, -1, -1,
                  -1, -1,  1, -1, -1
              });
    Tensor xp = model(xt, 20);
    xp.reshape(5, 5);
    xp.printValue2D();
    std::cout<<"H:"<<std::endl;
    Tensor xh({25, 1}, {
                   1,  1, -1, -1,  1,
                   1,  1, -1, -1,  1,
                   1,  1,  1,  1,  1,
                   1, -1,  1,  1,  1,
                   1, -1, -1,  1,  1
              });
    xp = model(xh, 20);
    xp.reshape(5, 5);
    xp.printValue2D();
    return;
}
int main()
{
    //test_kmeans();
    //test_kdtree();
    //test_svm();
    //test_gaussian();
    //test_hmm();
    test_hopfieldNet();
    return 0;
}
