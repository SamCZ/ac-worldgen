#pragma once

#include "blockside.h"

#include "util/hashutils.h"

namespace AC
{
	struct BlockOrientation {
		friend struct std::hash<BlockOrientation>;

	public:
		enum class TransformFlags {
			/// The result is translated so that it is adjacent to the block, not overlapping it
			adjacent = 1 << 0,

			/// The rotation happens on the left edge horizontally, not in the middle of the block
			horizontalEdge = 1 << 1,

			/// The rotation happens on the top edge vertically, not in the middle of the block
			verticalEdge = 1 << 2,

			/// Mirror the transform (horizontally)
			mirror = 1 << 3,
		};

	public:
		BlockOrientation();
		BlockOrientation(const std::string &facing, const std::string &rotation = {});
		BlockOrientation(BlockSide facing, BlockSide rotation);

	public:
		/// Returns adjacent/inverted block orientation
		BlockOrientation adjacent() const;

		/// Returns orientation with different up (calling this function four times reutrns the original orientation)
		BlockOrientation nextUpVariant() const;

		/// Returns transform you have to apply to match the target orientation
		BlockTransformMatrix transformToMatch(const BlockOrientation &target, int flags) const;

	public:
		inline bool isSpecified() const {
			return facing_ != BlockSide::_cnt;
		}

		inline bool isVertical() const {
			return absBlockSide(facing_) == BlockSide::Zplus;
		}

	public:
		std::string toString() const;

	public:
		inline bool operator ==(const BlockOrientation &other) const {
			return facing_ == other.facing_ && upDirection_ == other.upDirection_;
		}

	private:
		void validate();

	private:
		BlockSide facing_ = BlockSide::_cnt, upDirection_ = BlockSide::_cnt;

	};
}

template<>
struct std::hash<AC::BlockOrientation> {
	inline size_t operator ()(const AC::BlockOrientation &o) const {
		return HashUtils::multiHash(o.facing_, o.upDirection_);
	}
};