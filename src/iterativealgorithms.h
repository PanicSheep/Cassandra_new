#pragma once
#include <iostream>
#include <random>
#include <functional>
#include "vector_extensions.h"
#include "matrixCSR.h"

template <typename Matrix, typename Vector>
class IPreconditioner
{
public:
	virtual ~IPreconditioner() {}

	virtual Vector  Ax(const Vector& x) const = 0;
	virtual Vector ATx(const Vector& x) const = 0;
	virtual Vector backslash(const Vector& x) const = 0;

	inline  Vector operator*(const Vector& x) const { return Ax(x); }
};

template <typename Matrix, typename Vector>
class CDiagonalPreconditioner : public IPreconditioner<Matrix, Vector>
{
private:
	Vector m_P;
public:
	CDiagonalPreconditioner(Vector P) : m_P(std::move(P)) {}

	virtual Vector  Ax(const Vector& x) const { return m_P * x; }
	virtual Vector ATx(const Vector& x) const { return m_P * x; }
	virtual Vector backslash(const Vector& x) const { return x / m_P; }
};

enum eTerminationCause { None, MaxIteration, Converged };

template <typename Matrix, typename Vector, typename T = double>
class CIterativeAlgorithm
{
protected:
	const Matrix& A;
	const Vector& b;
	Vector x;
	double tolerance;
	std::vector<T> residuum;
	eTerminationCause terminationCause;
public:
	CIterativeAlgorithm() {}
	CIterativeAlgorithm(const Matrix& A, const Vector& b, Vector x0, double tolerance)
		: A(A), b(b), x(x0), tolerance(tolerance), terminationCause(eTerminationCause::None) {}
	virtual ~CIterativeAlgorithm() {}
	
	void SetValues(const Matrix& A_, const Vector& b_, Vector x0_, double tol)
	{
		A = A_;
		b = b_;
		x = x0_;
		tolerance = tol;
	}

	const std::vector<T>& GetResiduum() const { return residuum; }
	      std::vector<T>  GetResiduum()       { return residuum; }
	const Vector&         GetX()        const { return x; }
	      Vector          GetX()              { return x; }
	
	eTerminationCause GetTerminationCause() const { return terminationCause; }
	
	virtual void Iterate(std::size_t maxIteration) = 0;
};


/// Conjugate Gradient Method
/// Solves A * x = b for x
/// @param A has to be symmetric and positive-definite
/// @param b
/// @param x0 starting vector
/// @param tolerance stoping criteria for norm of residuum
template <typename Matrix, typename Vector, typename T = double>
class CIterativeCG : public CIterativeAlgorithm<Matrix, Vector, T>
{
protected:
	using CIterativeAlgorithm<Matrix, Vector, T>::A;
	using CIterativeAlgorithm<Matrix, Vector, T>::b;
	using CIterativeAlgorithm<Matrix, Vector, T>::tolerance;
	using CIterativeAlgorithm<Matrix, Vector, T>::residuum;
	using CIterativeAlgorithm<Matrix, Vector, T>::x;
	using CIterativeAlgorithm<Matrix, Vector, T>::terminationCause;
public:
	CIterativeCG() : CIterativeAlgorithm<Matrix, Vector, T>() {}
	CIterativeCG(const Matrix& A, const Vector& b, Vector x0, double tolerance)
		: CIterativeAlgorithm<Matrix, Vector, T>(A, b, x0, tolerance) {}
	
	void Iterate(std::size_t maxIteration)
	{
		assert(A.n() == A.m());
		assert(A.n() == b.size());
		assert(A.n() == x.size());
		assert(maxIteration >= 0);
		assert(tolerance >= 0);
		Vector r, p, A_p;
		float alpha, beta, r_dot_r_old, r_dot_r_new;

		r = b - A * x;
		p = r;
		r_dot_r_new = dot(r, r);
		residuum.push_back(r_dot_r_new);

		for (std::size_t k = 1; k <= maxIteration; k++)
		{		
			r_dot_r_old = r_dot_r_new;
			A_p = A * p;
			alpha = r_dot_r_old / dot(p, A_p);
			x += alpha * p;
			r -= alpha * A_p;
			r_dot_r_new = dot(r, r);
			residuum.push_back(r_dot_r_new);

			if (sqrt(r_dot_r_new) < tolerance) {
				terminationCause = eTerminationCause::Converged;
				return;
			}

			beta = r_dot_r_new / r_dot_r_old;
			p = r + beta * p;
		}
		terminationCause = eTerminationCause::MaxIteration;
	}
};

/// Preconditioned Conjugate Gradient Method
/// Solves A * P * y = b, where P * y = x, for x
/// @param A has to be symmetric and positive-definite
/// @param P Preconditioner
/// @param b
/// @param x0 starting vector
/// @param tolerance stoping criteria for norm of residuum
template <typename Matrix, typename Vector, typename T = double>
class CIterativePCG : public CIterativeAlgorithm<Matrix, Vector, T>
{
protected:
	using CIterativeAlgorithm<Matrix, Vector, T>::A;
	using CIterativeAlgorithm<Matrix, Vector, T>::b;
	using CIterativeAlgorithm<Matrix, Vector, T>::tolerance;
	using CIterativeAlgorithm<Matrix, Vector, T>::residuum;
	using CIterativeAlgorithm<Matrix, Vector, T>::x;
	using CIterativeAlgorithm<Matrix, Vector, T>::terminationCause;
	const IPreconditioner<Matrix, Vector> * P;
public:
	CIterativePCG() : CIterativeAlgorithm<Matrix, Vector, T>() {}
	CIterativePCG(const Matrix& A, const IPreconditioner<Matrix, Vector> * P, const Vector& b, Vector x0, double tolerance)
		: CIterativeAlgorithm<Matrix, Vector, T>(A, b, x0, tolerance), P(P) {}
	
	void Iterate(std::size_t maxIteration)
	{
		assert(A.n() == A.m());
		assert(A.n() == b.size());
		assert(A.n() == x.size());
		assert(maxIteration >= 0);
		assert(tolerance >= 0);
		Vector y, r, p, A_p;
		float alpha, beta, r_dot_r_old, r_dot_r_new;

		y = P->backslash(x);
		r = b - A * x;
		p = r;
		r_dot_r_new = dot(r, r);
		residuum.push_back(r_dot_r_new);

		for (std::size_t k = 1; k <= maxIteration; k++)
		{
			r_dot_r_old = r_dot_r_new;
			A_p = A * (*P * p);
			alpha = r_dot_r_old / dot(p, A_p);
			y += alpha * p;
			r -= alpha * A_p;
			r_dot_r_new = dot(r, r);
			residuum.push_back(r_dot_r_new);

			if (sqrt(r_dot_r_new) < tolerance) {
				x = *P * y;
				terminationCause = eTerminationCause::Converged;
				return;
			}
			
			beta = r_dot_r_new / r_dot_r_old;
			p = r + beta * p;
		}
		x = *P * y;
		terminationCause = eTerminationCause::MaxIteration;
	}
};

/// Conjugate Gradient Least Squares Method
/// Solves A' * A * x = A' * b for x
/// @param A
/// @param b
/// @param x0 starting vector
/// @param tolerance stoping criteria for norm of residuum
template <typename Matrix, typename Vector, typename T = double>
class CIterativeCGLS : public CIterativeAlgorithm<Matrix, Vector, T>
{
protected:
	using CIterativeAlgorithm<Matrix, Vector, T>::A;
	using CIterativeAlgorithm<Matrix, Vector, T>::b;
	using CIterativeAlgorithm<Matrix, Vector, T>::tolerance;
	using CIterativeAlgorithm<Matrix, Vector, T>::residuum;
	using CIterativeAlgorithm<Matrix, Vector, T>::x;
	using CIterativeAlgorithm<Matrix, Vector, T>::terminationCause;
public:
	CIterativeCGLS() : CIterativeAlgorithm<Matrix, Vector, T>() {}
	CIterativeCGLS(const Matrix& A, const Vector& b, Vector x0, double tolerance)
		: CIterativeAlgorithm<Matrix, Vector, T>(A, b, x0, tolerance) {}
	
	void Iterate(std::size_t maxIteration)
	{
		assert(A.n() == b.size());
		assert(A.m() == x.size());
		assert(maxIteration >= 0);
		assert(tolerance >= 0);
		Vector r, p, A_p;
		float alpha, beta, r_dot_r_old, r_dot_r_new;
		
		r = A.ATx(b - A * x);
		p = r;
		r_dot_r_new = dot(r, r);
		residuum.push_back(r_dot_r_new);

		for (std::size_t k = 1; k <= maxIteration; k++)
		{
			r_dot_r_old = r_dot_r_new;
			A_p = A.ATAx(p);
			alpha = r_dot_r_old / dot(p, A_p);
			x += alpha * p;
			r -= alpha * A_p;
			r_dot_r_new = dot(r, r);
			residuum.push_back(r_dot_r_new);

			if (sqrt(r_dot_r_new) < tolerance) {
				terminationCause = eTerminationCause::Converged;
				return;
			}
			
			beta = r_dot_r_new / r_dot_r_old;
			p = r + beta * p;
		}
		terminationCause = eTerminationCause::MaxIteration;
	}
};

/// Preconditioned Conjugate Gradient Least Squares Method
/// Solves A' * A * P * y = A' * b, where P * y = x, for x
/// @param A has to be symmetric and positive-definite
/// @param P Preconditioner
/// @param b
/// @param x0 starting vector
/// @param maxIteration maximum number of iterations
/// @param tolerance stoping criteria for norm of residuum
template <typename Matrix, typename Vector, typename T = double>
class CIterativePCGLS : public CIterativeAlgorithm<Matrix, Vector, T>
{
protected:
	using CIterativeAlgorithm<Matrix, Vector, T>::A;
	using CIterativeAlgorithm<Matrix, Vector, T>::b;
	using CIterativeAlgorithm<Matrix, Vector, T>::tolerance;
	using CIterativeAlgorithm<Matrix, Vector, T>::residuum;
	using CIterativeAlgorithm<Matrix, Vector, T>::x;
	using CIterativeAlgorithm<Matrix, Vector, T>::terminationCause;
	const IPreconditioner<Matrix, Vector> * P;
public:
	CIterativePCGLS() : CIterativeAlgorithm<Matrix, Vector, T>() {}
	CIterativePCGLS(const Matrix& A, const IPreconditioner<Matrix, Vector> * P, const Vector& b, Vector x0, double tolerance)
		: CIterativeAlgorithm<Matrix, Vector, T>(A, b, x0, tolerance), P(P) {}
	
	void Iterate(std::size_t maxIteration)
	{
		assert(A.n() == b.size());
		assert(A.m() == x.size());
		assert(maxIteration >= 0);
		assert(tolerance >= 0);
		Vector y, r, p, A_p;
		float alpha, beta, r_dot_r_old, r_dot_r_new;

		y = P->backslash(x);
		r = P->ATx(A.ATx(b - A * (*P * x)));
		p = r;
		r_dot_r_new = dot(r, r);
		residuum.push_back(r_dot_r_new);

		for (std::size_t k = 1; k <= maxIteration; k++)
		{
			r_dot_r_old = r_dot_r_new;
			A_p = P->ATx(A.ATAx(*P * p));
			alpha = r_dot_r_old / dot(p, A_p);
			y += alpha * p;
			r -= alpha * A_p;
			r_dot_r_new = dot(r, r);
			residuum.push_back(r_dot_r_new);

			if (sqrt(r_dot_r_new) < tolerance) {
				x = *P * y;
				terminationCause = eTerminationCause::Converged;
				return;
			}

			beta = r_dot_r_new / r_dot_r_old;
			p = r + beta * p;
		}
		x = *P * y;
		terminationCause = eTerminationCause::MaxIteration;
	}
};

/// Least Squares QR Method
/// Solves A * x = b for x
/// @param A
/// @param b
/// @param x0 starting vector
/// @param maxIteration maximum number of iterations
/// @param tolerance stoping criteria for norm of residuum
template <typename Matrix, typename Vector, typename T = double>
class CIterativeLSQR : public CIterativeAlgorithm<Matrix, Vector, T>
{
protected:
	using CIterativeAlgorithm<Matrix, Vector, T>::A;
	using CIterativeAlgorithm<Matrix, Vector, T>::b;
	using CIterativeAlgorithm<Matrix, Vector, T>::tolerance;
	using CIterativeAlgorithm<Matrix, Vector, T>::residuum;
	using CIterativeAlgorithm<Matrix, Vector, T>::x;
	using CIterativeAlgorithm<Matrix, Vector, T>::terminationCause;
public:
	CIterativeLSQR() : CIterativeAlgorithm<Matrix, Vector, T>() {}
	CIterativeLSQR(const Matrix& A, const Vector& b, Vector x0, double tolerance)
		: CIterativeAlgorithm<Matrix, Vector, T>(A, b, x0, tolerance) {}
	
	void Iterate(std::size_t maxIteration)
	{
		assert(A.n() == b.size());
		assert(maxIteration >= 0);
		assert(tolerance >= 0);
		Vector u, v, w;
		float alpha, beta, c, s, phi, rho, theta, phi_bar, rho_bar;

		Decompose(beta, u, b - A * x);
		Decompose(alpha, v, A.ATx(u));
		w = v;
		phi_bar = beta;
		rho_bar = alpha;
		rho = sqrt(rho_bar * rho_bar + beta * beta);
		s = beta / rho;
		residuum.push_back(phi_bar * alpha * std::abs(rho_bar / rho));

		for (std::size_t k = 1; k <= maxIteration; k++)
		{
			Decompose(beta, u, A * v - alpha * u);
			Decompose(alpha, v, A.ATx(u) - beta * v);

			rho = sqrt(rho_bar * rho_bar + beta * beta);
			c = rho_bar / rho;
			s = beta / rho;
			theta = s * alpha;
			rho_bar = -c * alpha;
			phi = c * phi_bar;
			phi_bar = s * phi_bar;
			x += (phi / rho) * w;
			w = v - (theta / rho) * w;
			residuum.push_back(phi_bar * alpha * std::abs(c));

			if (phi_bar * alpha * std::abs(c) < tolerance) {
				terminationCause = eTerminationCause::Converged;
				return;
			}
		}
		terminationCause = eTerminationCause::MaxIteration;
	}
};

/// Preconditioned Least Squares QR Method
/// Solves A * P * y = b with P * y = x for x
/// @param A
/// @param P Preconditioner
/// @param b
/// @param x0 starting vector
/// @param maxIteration maximum number of iterations
/// @param tolerance stoping criteria for norm of residuum
template <typename Matrix, typename Vector, typename T = double>
class CIterativePLSQR : public CIterativeAlgorithm<Matrix, Vector, T>
{
protected:
	using CIterativeAlgorithm<Matrix, Vector, T>::A;
	using CIterativeAlgorithm<Matrix, Vector, T>::b;
	using CIterativeAlgorithm<Matrix, Vector, T>::tolerance;
	using CIterativeAlgorithm<Matrix, Vector, T>::residuum;
	using CIterativeAlgorithm<Matrix, Vector, T>::x;
	using CIterativeAlgorithm<Matrix, Vector, T>::terminationCause;
	const IPreconditioner<Matrix, Vector> * P;
public:
	CIterativePLSQR() : CIterativeAlgorithm<Matrix, Vector, T>() {}
	CIterativePLSQR(const Matrix& A, const IPreconditioner<Matrix, Vector> * P, const Vector& b, Vector x0, double tolerance)
		: CIterativeAlgorithm<Matrix, Vector, T>(A, b, x0, tolerance), P(P) {}
	
	void Iterate(std::size_t maxIteration)
	{
		assert(A.n() == b.size());
		assert(maxIteration >= 0);
		assert(tolerance >= 0);
		Vector y, u, v, w;
		float alpha, beta, c, s, phi, rho, theta, phi_bar, rho_bar;

		y = P->backslash(x);
		Decompose(beta, u, b - A * x);
		Decompose(alpha, v, P->ATx(A.ATx(u)));
		w = v;
		phi_bar = beta;
		rho_bar = alpha;
		rho = sqrt(rho_bar * rho_bar + beta * beta);
		s = beta / rho;
		residuum.push_back(phi_bar * alpha * std::abs(rho_bar / rho));

		for (std::size_t k = 1; k <= maxIteration; k++)
		{
			Decompose(beta, u, A * (*P * v) - alpha * u);
			Decompose(alpha, v, P->ATx(A.ATx(u)) - beta * v);

			rho = sqrt(rho_bar * rho_bar + beta * beta);
			c = rho_bar / rho;
			s = beta / rho;
			theta = s * alpha;
			rho_bar = -c * alpha;
			phi = c * phi_bar;
			phi_bar = s * phi_bar;
			y += (phi / rho) * w;
			w = v - (theta / rho) * w;
			residuum.push_back(phi_bar * alpha * std::abs(c));

			if (phi_bar * alpha * std::abs(c) < tolerance) {
				x = *P * y;
				terminationCause = eTerminationCause::Converged;
				return;
			}
		}
		x = *P * y;
		terminationCause = eTerminationCause::MaxIteration;
	}
};
