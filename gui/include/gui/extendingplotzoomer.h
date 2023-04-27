/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see http://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef EXTENDINGPLOTZOOMER_H
#define EXTENDINGPLOTZOOMER_H

#include "limitedplotzoomer.h"

#include <qwt_plot_shapeitem.h>
#include "scopygui_export.h"

namespace scopy {
class SCOPYGUI_EXPORT ExtendingPlotZoomer : public LimitedPlotZoomer
{
public:
	explicit ExtendingPlotZoomer(QWidget*, bool doReplot = false);
	virtual ~ExtendingPlotZoomer();

protected:
	virtual QPolygon adjustedPoints(const QPolygon &) const;
	virtual void zoom(const QRectF &);

private:
	mutable bool widthPass;
	mutable bool heightPass;
	mutable bool extendWidth;
	mutable bool extendHeight;

	QVector<QwtPlotShapeItem*> cornerMarkers;
	QVector<QwtPlotShapeItem*> extendMarkers;
};
}

#endif // EXTENDINGPLOTZOOMER_H