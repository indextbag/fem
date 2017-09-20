// OpenTissue Template Library
//
// A generic toolbox for physics based modeling and simulation.
// Copyright (c) 2008 Department of Computer Science, University of Copenhagen.
//
// OTTL is licensed under zlib: http://opensource.org/licenses/zlib-license.php.

#ifndef OPENTISSUE_MATH_MATRIX_DECOMPOSITION_H
#define OPENTISSUE_MATH_MATRIX_DECOMPOSITION_H

#include <opentissue/configuration.h>

#include <cmath>

namespace opentissue {
    namespace math {
        template <typename matrix_type, typename vector_type>
        inline void eigen(matrix_type const &A, matrix_type &V,
            vector_type &diag) {
            typedef typename vector_type::value_type real_type;
            typedef typename vector_type::value_traits value_traits;

            using std::fabs;
            using std::sqrt;

            vector_type sub_diag;

            sub_diag.clear();
            diag.clear();

            V = A;

            real_type const &fM00 = V(0, 0);
            real_type fM01 = V(0, 1);
            real_type fM02 = V(0, 2);
            real_type const &fM11 = V(1, 1);
            real_type const &fM12 = V(1, 2);
            real_type const &fM22 = V(2, 2);

            diag(0) = fM00;
            sub_diag(2) = value_traits::zero();
            if (fM02 != value_traits::zero()) {
                real_type fLength = sqrt(fM01 * fM01 + fM02 * fM02);
                real_type fInvLength = (value_traits::one()) / fLength;
                fM01 *= fInvLength;
                fM02 *= fInvLength;
                real_type fQ = (value_traits::two()) * fM01 * fM12 + fM02 * (fM22 - fM11);
                diag(1) = fM11 + fM02 * fQ;
                diag(2) = fM22 - fM02 * fQ;
                sub_diag(0) = fLength;
                sub_diag(1) = fM12 - fM01 * fQ;
                V(0, 0) = value_traits::one();
                V(0, 1) = value_traits::zero();
                V(0, 2) = value_traits::zero();
                V(1, 0) = value_traits::zero();
                V(1, 1) = fM01;
                V(1, 2) = fM02;
                V(2, 0) = value_traits::zero();
                V(2, 1) = fM02;
                V(2, 2) = -fM01;
            }
            else {
                diag(1) = fM11;
                diag(2) = fM22;
                sub_diag(0) = fM01;
                sub_diag(1) = fM12;
                V(0, 0) = value_traits::one();
                V(0, 1) = value_traits::zero();
                V(0, 2) = value_traits::zero();
                V(1, 0) = value_traits::zero();
                V(1, 1) = value_traits::one();
                V(1, 2) = value_traits::zero();
                V(2, 0) = value_traits::zero();
                V(2, 1) = value_traits::zero();
                V(2, 2) = value_traits::one();
            }

            const int max_iterations = 32;
            const int dim = 3;
            for (int i0 = 0; i0 < dim; ++i0) {
                int i1;
                for (i1 = 0; i1 < max_iterations; ++i1) {
                    int i2;
                    for (i2 = i0; i2 <= dim - 2; ++i2) {
                        real_type fTmp = fabs(diag(i2)) + fabs(diag(i2 + 1));
                        if (fabs(sub_diag(i2)) + fTmp == fTmp)
                            break;
                    }
                    if (i2 == i0)
                        break;
                    real_type fG =
                        (diag(i0 + 1) - diag(i0)) / (value_traits::two() * sub_diag(i0));
                    real_type fR = sqrt(fG * fG + value_traits::one());
                    if (fG < value_traits::zero())
                        fG = diag(i2) - diag(i0) + sub_diag(i0) / (fG - fR);
                    else
                        fG = diag(i2) - diag(i0) + sub_diag(i0) / (fG + fR);

                    real_type fSin = value_traits::one();
                    real_type fCos = value_traits::one();
                    real_type fP = value_traits::zero();

                    for (int i3 = i2 - 1; i3 >= i0; --i3) {
                        real_type fF = fSin * sub_diag(i3);
                        real_type fB = fCos * sub_diag(i3);
                        if (fabs(fF) >= fabs(fG)) {
                            fCos = fG / fF;
                            fR = sqrt(fCos * fCos + value_traits::one());
                            sub_diag(i3 + 1) = fF * fR;
                            fSin = value_traits::one() / fR;
                            fCos *= fSin;
                        }
                        else {
                            fSin = fF / fG;
                            fR = sqrt(fSin * fSin + value_traits::one());
                            sub_diag(i3 + 1) = fG * fR;
                            fCos = value_traits::one() / fR;
                            fSin *= fCos;
                        }
                        fG = diag(i3 + 1) - fP;
                        fR = (diag(i3) - fG) * fSin + value_traits::two() * fB * fCos;
                        fP = fSin * fR;
                        diag(i3 + 1) = fG + fP;
                        fG = fCos * fR - fB;
                        for (int i4 = 0; i4 < dim; ++i4) {
                            fF = V(i4, i3 + 1);
                            V(i4, i3 + 1) = fSin * V(i4, i3) + fCos * fF;
                            V(i4, i3) = fCos * V(i4, i3) - fSin * fF;
                        }
                    }
                    diag(i0) -= fP;
                    sub_diag(i0) = fG;
                    sub_diag(i2) = value_traits::zero();
                }
                if (i1 == max_iterations)
                    break;
            }
        }

		template <typename matrix_type, typename real_type>
		inline void polar_decomposition(matrix_type const &A, unsigned int max_iterations,
			real_type const &tolerance, matrix_type &U,
			matrix_type &P) {
			typedef typename matrix_type::value_type real_type;

			using std::fabs;
			using std::sqrt;

			U = A;
			P = A.inverse();

			for (unsigned int i = 0; i < imax; i++) {
				Float uOneNorm = u.oneNorm();

				Float uNorm = u.infinityNorm() * uOneNorm;
				Float pNorm = p.infinityNorm() * p.oneNorm();

				if (uNorm == 0 || pNorm == 0)
					break;

				Float g = math::pow(pNorm / uNorm, 0.25);
				math::Matrix d = 0.5 * (u * (g - 2.0) + p.transposed() / g);

				u += d;
				p = u.inverse();

				if (d.oneNorm() <= eps * uOneNorm) {
					p = u.transposed() * a;
					p = 0.5 * (p + p.transposed());

					return;
				}
			}

			p = u.transposed() * a;
			p = 0.5 * (p + p.transposed());
		}
		
		template <typename matrix_type>
		inline void qr_decomposition(matrix_type const &A, matrix_type &Q,
			matrix_type &R) {
			typedef typename matrix_type::value_type real_type;
			typedef typename matrix_type::value_traits value_traits;
			typedef typename matrix_type::vector_type vector_type;

			using std::fabs;

			Q(0, 0) = value_traits::one();
			Q(0, 1) = value_traits::zero();
			Q(0, 2) = value_traits::zero();
			Q(1, 0) = value_traits::zero();
			Q(1, 1) = value_traits::one();
			Q(1, 2) = value_traits::zero();
			Q(2, 0) = value_traits::zero();
			Q(2, 1) = value_traits::zero();
			Q(2, 2) = value_traits::one();

			R = A;

			for (unsigned int i = 0; i < 3; i++) {
				vector_type v;

				for (unsigned j = i; j < 3; j++)
					v(j) = R(j, i);

				v(i) += math::sign(v[i]) * math::length(v);

				real_type m = math::length(v);

				if (m == 0)
					continue;

				v /= m;

				matrix_type H;

				H[0][0] = 1.0 - 2.0 * v[0] * v[0];
				H[0][1] = -2.0 * v[0] * v[1];
				H[0][2] = -2.0 * v[0] * v[2];
				H[1][0] = H[0][1];
				H[1][1] = 1.0 - 2.0 * v[1] * v[1];
				H[1][2] = -2.0 * v[1] * v[2];
				H[2][0] = H[0][2];
				H[2][1] = H[1][2];
				H[2][2] = 1.0 - 2.0 * v[2] * v[2];

				R = H * R;
				Q *= H;
			}
		}
    }
}

#endif