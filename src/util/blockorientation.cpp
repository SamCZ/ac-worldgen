#include "blockorientation.h"

static const QHash<QString, BlockSide> strToSide{
	{"x+",      BlockSide::Xplus},
	{"x-",      BlockSide::Xminus},
	{"y+",      BlockSide::Yplus},
	{"y-",      BlockSide::Yminus},
	{"z+",      BlockSide::Zplus},
	{"z-",      BlockSide::Zminus},
	{QString(), BlockSide::_cnt}
};

BlockOrientation::BlockOrientation() {

}

BlockOrientation::BlockOrientation(const QString &facing, const QString &rotation) {
	facing_ = strToSide[facing];
	upDirection_ = strToSide[rotation];

	validate();
}

BlockOrientation::BlockOrientation(BlockSide facing, BlockSide rotation) {
	facing_ = facing;
	upDirection_ = rotation;

	validate();
}

BlockOrientation BlockOrientation::adjacent() const {
	return isSpecified() ? BlockOrientation(oppositeBlockSide(facing_), upDirection_) : BlockOrientation();
}

BlockOrientation BlockOrientation::nextUpVariant() const {
	if(!isSpecified())
		return BlockOrientation();

	int up = +upDirection_;

	do
		up = (+up + 1) % +BlockSide::_cnt;

	while(absBlockSide(facing_) == absBlockSide(static_cast<BlockSide>(up)));

	return BlockOrientation(facing_, static_cast<BlockSide>(up));
}

BlockTransformMatrix BlockOrientation::transformToMatch(const BlockOrientation &target, int flags) const {
	if(!isSpecified() || !target.isSpecified())
		return BlockTransformMatrix();

	static const QHash<BlockOrientation, BlockTransformMatrix> matrices = [] {
		QHash<BlockOrientation, BlockTransformMatrix> result;

		for(int f = 0; f < +BlockSide::_cnt; f++) {
			const BlockSide fs = static_cast<BlockSide>(f);

			for(int u = 0; u < +BlockSide::_cnt; u++) {
				const BlockSide us = static_cast<BlockSide>(u);

				if(absBlockSide(fs) == absBlockSide(us))
					continue;

				result.insert(BlockOrientation(fs, us), BlockTransformMatrix::lookAt(BlockWorldPos(), blockSideNormalVector(f),
				                                                                     blockSideNormalVector(u)));
			}
		}

		return result;
	}();
	static const QHash<BlockOrientation, BlockTransformMatrix> invMatrices = [] {
		QHash<BlockOrientation, BlockTransformMatrix> result;
		for(auto it = matrices.begin(), end = matrices.end(); it != end; it++)
			result.insert(it.key(), it.value().nonScalingInverted());
		return result;
	}();

	BlockTransformMatrix result;

	result *= invMatrices.value(target);

	const BlockWorldPos translation = BlockWorldPos(
		(flags & +TransformFlags::horizontalEdge) && !(flags & +TransformFlags::mirror) ? 1 : 0,
		flags & +TransformFlags::verticalEdge ? 1 : 0,
		flags & +TransformFlags::adjacent ? 1 : 0
	);

	if(translation != BlockWorldPos())
		result *= BlockTransformMatrix::translation(translation);

	if(flags & +TransformFlags::mirror)
		result *= BlockTransformMatrix::scaling(BlockWorldPos(-1, 1, 1));

	result *= matrices[*this];

	return result;
}

QString BlockOrientation::toString() const {
	if(!isSpecified())
		return QStringLiteral("(unspecified)");

	static const QHash<int, QString> sideToStr = [] {
		QHash<int, QString> result;
		for(auto it = strToSide.begin(), end = strToSide.end(); it != end; it++)
			result[+it.value()] = it.key();

		return result;
	}();

	return sideToStr[+facing_] + sideToStr[+upDirection_];
}

void BlockOrientation::validate() {
	if(isSpecified() && upDirection_ == BlockSide::_cnt)
		upDirection_ = absBlockSide(facing_) == BlockSide::Zplus ? BlockSide::Xplus : BlockSide::Zplus;

	ASSERT(!isSpecified() || absBlockSide(facing_) != absBlockSide(upDirection_));
}