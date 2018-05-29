#pragma once
#include <cstdint>
#include <memory>
#include <vector>
#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <condition_variable>

#include "PuzzleCollection.h"
#include "Path.h"
#include "Serializable.h"


namespace IO
{
	class FilePuzzleCollection : public PuzzleCollection
	{
	public:
		FilePuzzleCollection() {}
		FilePuzzleCollection(const CPath& file);
		template <typename ITERATOR> FilePuzzleCollection(ITERATOR begin, ITERATOR end) : m_puzzles(begin, end) {}

		virtual void Load(const CPath& file);
		virtual void Save(const CPath& file) const;
	};


	class AutoSavingPuzzleCollection : public FilePuzzleCollection
	{
		std::atomic_bool m_terminate;
		std::condition_variable m_cv;
		std::thread m_thread;

		mutable std::mutex m_mtx;

	public:
		AutoSavingPuzzleCollection() {}
		AutoSavingPuzzleCollection(const CPath& file, std::chrono::seconds interval) : AutoSavingPuzzleCollection(file, file, interval) {}
		AutoSavingPuzzleCollection(const CPath& input_file, const CPath& output_file, std::chrono::seconds interval);
		~AutoSavingPuzzleCollection();

		void Load(const CPath& file) override;
		void Save(const CPath& file) const override;

		void push_back(std::unique_ptr<CPuzzle>&& pos) override;

		std::unique_ptr<CPuzzle> Get(std::size_t index) const override;
		void Set(std::size_t index, std::unique_ptr<CPuzzle>&& pos) override;

		std::size_t size() const override;
	};
}