//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2016 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include "lomse_graphic_view.h"

#include <cstdio>       //for sprintf
#include "lomse_graphical_model.h"
#include "lomse_gm_basic.h"
#include "lomse_screen_drawer.h"
#include "lomse_interactor.h"
#include "lomse_caret.h"
#include "lomse_caret_positioner.h"
#include "lomse_logger.h"
#include "lomse_time_grid.h"
#include "lomse_visual_effect.h"
#include "lomse_overlays_generator.h"
#include "lomse_handler.h"

using namespace std;

namespace lomse
{


//=======================================================================================
// ViewFactory implementation
//=======================================================================================
ViewFactory::ViewFactory()
{
}

//---------------------------------------------------------------------------------------
ViewFactory::~ViewFactory()
{
}

//---------------------------------------------------------------------------------------
View* ViewFactory::create_view(LibraryScope& libraryScope, int viewType,
                               ScreenDrawer* pDrawer)
{
    switch(viewType)
    {
        case k_view_simple:
            return LOMSE_NEW SimpleView(libraryScope, pDrawer);

        case k_view_vertical_book:
            return LOMSE_NEW VerticalBookView(libraryScope, pDrawer);

        case k_view_horizontal_book:
            return LOMSE_NEW HorizontalBookView(libraryScope, pDrawer);

        default:
        {
            LOMSE_LOG_ERROR("[ViewFactory::create_view] invalid view type");
            throw runtime_error("[ViewFactory::create_view] invalid view type");
        }
    }
    return NULL;
}


//=======================================================================================
// GraphicView implementation
//=======================================================================================
GraphicView::GraphicView(LibraryScope& libraryScope, ScreenDrawer* pDrawer)
    : View()
    , m_libraryScope(libraryScope)
    , m_pDrawer(pDrawer)
    , m_options()
    , m_pRenderBuf(NULL)

    , m_expand(0.0)
    , m_gamma(1.0)
    , m_rotation(0.0)   //degrees: -180.0 to 180.0
    , m_transform()
    , m_vxOrg(0)
    , m_vyOrg(0)
    , m_pCaret(NULL)
    , m_pCursor(NULL)
    , m_fTempoLineVisible(false)
{
    m_pCaret = LOMSE_NEW Caret(this, libraryScope);
    m_pDragImg = LOMSE_NEW DraggedImage(this, libraryScope);
    m_pSelRect = LOMSE_NEW SelectionRectangle(this, libraryScope);
    m_pHighlighted = LOMSE_NEW PlaybackHighlight(this, libraryScope);
    m_pTimeGrid = LOMSE_NEW TimeGrid(this, m_libraryScope);

    m_pOverlaysGenerator = LOMSE_NEW OverlaysGenerator(this, libraryScope);

    add_visual_effect(m_pCaret);
    add_visual_effect(m_pDragImg);
    add_visual_effect(m_pHighlighted);
    add_visual_effect(m_pSelRect);
    //add_visual_effect(m_pTimeGrid);
}

//---------------------------------------------------------------------------------------
GraphicView::~GraphicView()
{
    delete m_pDrawer;
    delete m_pOverlaysGenerator;

    //ownership of VisualEffects is transferred to OverlaysGenerator
//    delete m_pCaret;
//    delete m_pDragImg;
//    delete m_pHighlighted;
//    delete m_pSelRect;
//    delete m_pTimeGrid;
//    delete_all_handlers();
}

//---------------------------------------------------------------------------------------
void GraphicView::use_cursor(DocCursor* pCursor)
{
    m_pCursor = pCursor;
}

//---------------------------------------------------------------------------------------
void GraphicView::use_selection_set(SelectionSet* pSelectionSet)
{
    m_pSelObjects = LOMSE_NEW SelectionHighlight(this, m_libraryScope, pSelectionSet);
    add_visual_effect(m_pSelObjects);
}

//---------------------------------------------------------------------------------------
void GraphicView::add_visual_effect(VisualEffect* pEffect)
{
    m_pOverlaysGenerator->add_visual_effect(pEffect);
}

//---------------------------------------------------------------------------------------
void GraphicView::set_visual_effects_for_mode(int mode)
{
    bool fEditionMode = (mode == Interactor::k_mode_edition);
    bool fPlaybackMode = (mode == Interactor::k_mode_playback);

    //effects only visible in edition mode:
    m_pCaret->set_visible(fEditionMode);
    m_pTimeGrid->set_visible(fEditionMode);
    m_pSelObjects->set_visible(fEditionMode);

    //effects only visible in playback mode:
    m_pHighlighted->set_visible(fPlaybackMode);
    //m_pTempoLine->set_visible(fPlaybackMode);

    //effects dynamically controlled
    m_pSelRect->set_visible(false);
    m_pDragImg->set_visible(false);

    //any other visibility constraints are determined at layout time:
    //Handlers, TimeGrid
    if (!fEditionMode)
        delete_all_handlers();
}

//---------------------------------------------------------------------------------------
void GraphicView::layout_selection_highlight()
{
    if (m_pSelObjects->is_visible())
    {
        if (m_pSelObjects->are_handlers_needed())
        {
            GmoObj* pGmo = m_pSelObjects->get_object_needing_handlers();
            if (m_pOverlaysGenerator->get_handlers_owner() != pGmo)
            {
                delete_all_handlers();
                int numHandlers = pGmo->get_num_handlers();
                for (int i=0; i < numHandlers; ++i)
                {
                    add_handler(i, pGmo);
                }
                m_pOverlaysGenerator->set_handlers_owner(pGmo);
            }
        }
        else
            delete_all_handlers();
    }
}

//---------------------------------------------------------------------------------------
void GraphicView::delete_all_handlers()
{
    list<Handler*>::iterator it = m_handlers.begin();
    while (it != m_handlers.end())
    {
        Handler* pHandler = *it;
        m_pOverlaysGenerator->remove_visual_effect(pHandler);
        it = m_handlers.erase(it);
        delete pHandler;
    }
    m_pOverlaysGenerator->set_handlers_owner(NULL);
}

//---------------------------------------------------------------------------------------
void GraphicView::add_handler(int iHandler, GmoObj* pOwnerGmo)
{
    Handler* pHandler =
        LOMSE_NEW HandlerCircle(this, m_libraryScope, pOwnerGmo, iHandler);
    pHandler->set_visible(true);
    pHandler->move_to( pOwnerGmo->get_handler_point(iHandler) );
    m_handlers.push_back(pHandler);
    add_visual_effect(pHandler);
}

//---------------------------------------------------------------------------------------
void GraphicView::new_viewport(Pixels x, Pixels y)
{
    stringstream s;
    s << "x=" << x << ", y=" << y;
    LOMSE_LOG_DEBUG(Logger::k_mvc, s.str());

    m_vxOrg = x;
    m_vyOrg = y;
    m_transform.tx = double(-x);
    m_transform.ty = double(-y);
}

//---------------------------------------------------------------------------------------
GraphicModel* GraphicView::get_graphic_model()
{
    return m_pInteractor->get_graphic_model();
}

////---------------------------------------------------------------------------------------
//void GraphicView::on_resize(Pixels vx, Pixels vy)
//{
//    set_viewport_at_page_center(vx);
//}

//---------------------------------------------------------------------------------------
void GraphicView::redraw_bitmap() //, RepaintOptions& opt)
{
    LOMSE_LOG_DEBUG(Logger::k_render, "");

    draw_all();
}

//---------------------------------------------------------------------------------------
void GraphicView::show_caret()
{
    m_pCaret->show_caret();
}

//---------------------------------------------------------------------------------------
void GraphicView::hide_caret()
{
    m_pCaret->hide_caret();
}

//---------------------------------------------------------------------------------------
void GraphicView::toggle_caret()
{
    if ( m_pCaret->is_visible() && m_pCaret->is_blink_enabled() )
        m_pCaret->toggle_caret();
}

//---------------------------------------------------------------------------------------
void GraphicView::layout_caret()
{
    GraphicModel* pGModel = get_graphic_model();
    CaretPositioner positioner;
    positioner.layout_caret(m_pCaret, m_pCursor, pGModel);
}

//---------------------------------------------------------------------------------------
bool GraphicView::is_caret_visible()
{
    return m_pCaret->is_visible();
}

//---------------------------------------------------------------------------------------
bool GraphicView::is_caret_blink_enabled()
{
    return m_pCaret->is_blink_enabled();
}

//---------------------------------------------------------------------------------------
void GraphicView::move_drag_image(LUnits x, LUnits y)
{
    m_pDragImg->move_to(x, y);
    draw_dragged_image();
}

//---------------------------------------------------------------------------------------
void GraphicView::show_drag_image(bool fShow)
{
    m_pDragImg->set_visible(fShow);
}

//---------------------------------------------------------------------------------------
void GraphicView::enable_drag_image(bool fEnabled)
{
    m_pDragImg->enable(fEnabled);
}

//---------------------------------------------------------------------------------------
void GraphicView::set_drag_image(GmoShape* pShape, bool fGetOwnership, UPoint offset)
{
    m_pDragImg->set_shape(pShape, fGetOwnership, offset);
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_all()
{
    if (m_pRenderBuf)
    {
        LOMSE_LOG_DEBUG(Logger::k_mvc, "");

        //render graphical model
        m_pInteractor->timing_start_measurements();
        draw_graphic_model();
        m_pInteractor->timing_graphic_model_render_end();

        //render handlers and visual effects
        m_pInteractor->timing_visual_effects_start();
        m_pOverlaysGenerator->on_new_background();
        draw_all_visual_effects();
        m_pInteractor->timing_renderization_end();
    }
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_caret()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_pInteractor->timing_start_measurements();
    layout_caret();
    layout_time_grid();                 //depends on caret
    layout_selection_highlight();       //for hidding Handlers
    m_pOverlaysGenerator->update_visual_effect(m_pCaret, m_pDrawer);
    m_pInteractor->timing_renderization_end();
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_time_grid()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_pInteractor->timing_start_measurements();
    layout_time_grid();
    m_pOverlaysGenerator->update_visual_effect(m_pTimeGrid, m_pDrawer);
    m_pInteractor->timing_renderization_end();
}

//---------------------------------------------------------------------------------------
void GraphicView::layout_time_grid()
{
    if (m_pCaret->is_visible()
        && m_pCursor->is_inside_terminal_node()
        && m_pCursor->get_parent_object()->is_score())
    {
        m_pTimeGrid->set_system( m_pCaret->get_active_system() );
        m_pTimeGrid->set_visible(true);
    }
    else
        m_pTimeGrid->set_visible(false);
}

//---------------------------------------------------------------------------------------
DocCursorState GraphicView::click_event_to_cursor_state(int iPage, LUnits x, LUnits y,
                                                        ImoObj* pImo, GmoObj* pGmo)
{
    GraphicModel* pGModel = get_graphic_model();
    CaretPositioner positioner;
    return positioner.click_point_to_cursor_state(pGModel, iPage, x, y, pImo, pGmo);
}

//---------------------------------------------------------------------------------------
string GraphicView::get_caret_timecode()
{
    return m_pCaret->get_timecode();
}

//---------------------------------------------------------------------------------------
void GraphicView::on_print_page(int page, double scale, VPoint viewport)
{
    //save and replace scale
    double screenScale = get_scale();
    set_scale(scale);

    //draw page
    m_pDrawer->reset(*m_pPrintBuf, Color(255, 255, 255));
    m_pDrawer->set_viewport(viewport.x, viewport.y);
    m_pDrawer->set_transform(m_transform);

    UPoint origin(0.0f, 0.0f);
    GraphicModel* pGModel = get_graphic_model();
    pGModel->draw_page(page, origin, m_pDrawer, m_options);
    m_pDrawer->render();

    //restore scale
    set_scale(screenScale);
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_graphic_model()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_options.background_color = Color(145, 156, 166);  //35, 52, 91); //127,127,127);
    m_options.page_border_flag = true;
    m_options.cast_shadow_flag = true;
    m_options.draw_anchors = m_libraryScope.draw_anchors();
    m_options.draw_shape_bounds = m_libraryScope.draw_shape_bounds();
    m_options.read_only_mode =
        m_pInteractor->get_operating_mode() != Interactor::k_mode_edition;

    m_pDrawer->reset(*m_pRenderBuf, m_options.background_color);
    m_pDrawer->set_viewport(m_vxOrg, m_vyOrg);
    m_pDrawer->set_transform(m_transform);

    generate_paths();
    m_pDrawer->render();
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_tempo_line()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    if (m_fTempoLineVisible)
    {
        //m_pInteractor->timing_start_measurements();
        double x1 = double( m_tempoLine.left() );
        double y1 = double( m_tempoLine.top() );
        double x2 = double( m_tempoLine.right() );
        double y2 = double( m_tempoLine.bottom() );

        m_pDrawer->screen_point_to_model(&x1, &y1);
        m_pDrawer->screen_point_to_model(&x2, &y2);

        double line_width = double( m_pDrawer->Pixels_to_LUnits(1) );

        m_pDrawer->begin_path();
        m_pDrawer->fill( Color(0, 255, 0) );        //solid green
        m_pDrawer->stroke( Color(0, 255, 0) );      //solid green
        m_pDrawer->stroke_width(line_width);
        m_pDrawer->move_to(x1, y1);
        m_pDrawer->hline_to(x2);
        m_pDrawer->vline_to(y2);
        m_pDrawer->hline_to(x1);
        m_pDrawer->vline_to(y1);
        m_pDrawer->end_path();

        m_pDrawer->render();
        //m_pInteractor->timing_renderization_end();
    }
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_all_visual_effects()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    layout_caret();
    layout_time_grid();
    layout_selection_highlight();
    m_pOverlaysGenerator->update_all_visual_effects(m_pDrawer);
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_selection_rectangle()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_pInteractor->timing_start_measurements();
    m_pOverlaysGenerator->update_visual_effect(m_pSelRect, m_pDrawer);
    m_pInteractor->timing_renderization_end();
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_playback_highlight()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_pInteractor->timing_start_measurements();
    m_pOverlaysGenerator->update_visual_effect(m_pHighlighted, m_pDrawer);
    m_pInteractor->timing_renderization_end();
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_dragged_image()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_pInteractor->timing_start_measurements();
    m_pOverlaysGenerator->update_visual_effect(m_pDragImg, m_pDrawer);
    m_pInteractor->timing_renderization_end();
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_handler(Handler* pHandler)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_pInteractor->timing_start_measurements();
    m_pOverlaysGenerator->update_visual_effect(pHandler, m_pDrawer);
    m_pInteractor->timing_renderization_end();
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_selected_objects()
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    m_pInteractor->timing_start_measurements();
    layout_selection_highlight();
    m_pOverlaysGenerator->update_visual_effect(m_pSelObjects, m_pDrawer);
    m_pInteractor->timing_renderization_end();
}

//---------------------------------------------------------------------------------------
VRect GraphicView::get_damaged_rectangle()
{
    URect uRect = m_pOverlaysGenerator->get_damaged_rectangle();
    if (uRect == URect(0.0, 0.0, 0.0, 0.0))
        return VRect(0, 0, 0, 0);

    double left = uRect.left();
    double top = uRect.top();
    double right = uRect.right();
    double bottom = uRect.bottom();
    m_pDrawer->model_point_to_screen(&left, &top);
    m_pDrawer->model_point_to_screen(&right, &bottom);

    //trim rectangle
    Pixels x1 = max(0, Pixels(left));
    Pixels y1 = max(0, Pixels(top));
    Pixels x2 = min(Pixels(right), int(m_pRenderBuf->width()) );
    Pixels y2 = min(Pixels(bottom), int(m_pRenderBuf->height()) );

    return VRect(VPoint(x1, y1), VPoint(x2, y2));
}

//---------------------------------------------------------------------------------------
void GraphicView::highlight_object(ImoStaffObj* pSO)
{
    GraphicModel* pGModel = get_graphic_model();
    m_pHighlighted->set_visible(true);
    m_pHighlighted->add_highlight( pGModel->get_main_shape_for_imo(pSO->get_id()) );
}

//---------------------------------------------------------------------------------------
void GraphicView::remove_highlight_from_object(ImoStaffObj* pSO)
{
    GraphicModel* pGModel = get_graphic_model();
    m_pHighlighted->remove_highlight( pGModel->get_main_shape_for_imo(pSO->get_id()) );
}

//---------------------------------------------------------------------------------------
void GraphicView::remove_all_highlight()
{
    m_pHighlighted->set_visible(false);
    m_pHighlighted->remove_all_highlight();
}

//---------------------------------------------------------------------------------------
Handler* GraphicView::handlers_hit_test(LUnits x, LUnits y)
{
    list<Handler*>::const_iterator it;
    for (it = m_handlers.begin(); it != m_handlers.end(); ++it)
    {
        if ((*it)->hit_test(x, y))
            return *it;
    }
    return NULL;
}

//---------------------------------------------------------------------------------------
void GraphicView::start_selection_rectangle(LUnits x1, LUnits y1)
{
    m_pSelRect->set_start_point(x1, y1);
    m_pSelRect->set_end_point(x1, y1);
    m_pSelRect->set_visible(true);
}

//---------------------------------------------------------------------------------------
void GraphicView::hide_selection_rectangle()
{
    m_pSelRect->set_visible(false);
}

//---------------------------------------------------------------------------------------
void GraphicView::update_selection_rectangle(LUnits x2, LUnits y2)
{
    m_pSelRect->set_end_point(x2, y2);
}

//---------------------------------------------------------------------------------------
void GraphicView::show_tempo_line(Pixels x1, Pixels y1, Pixels x2, Pixels y2)
{
    m_fTempoLineVisible = true;
    m_tempoLine.left(x1);
    m_tempoLine.top(y1);
    m_tempoLine.right(x2);
    m_tempoLine.bottom(y2);
}

//---------------------------------------------------------------------------------------
void GraphicView::hide_tempo_line()
{
    m_fTempoLineVisible = false;
}

//---------------------------------------------------------------------------------------
void GraphicView::update_tempo_line(Pixels x2, Pixels y2)
{
    m_tempoLine.right(x2);
    m_tempoLine.bottom(y2);
}

//---------------------------------------------------------------------------------------
void GraphicView::zoom_in(Pixels x, Pixels y)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    double rx(x);
    double ry(y);

    //move origin (left-top window corner) to rx, ry
    m_transform *= agg::trans_affine_translation(-rx, -ry);

    //apply scaling
    m_transform *= agg::trans_affine_scaling(1.05);

    //move origin back to (rx, ry) so this point remains un-moved
    m_transform *= agg::trans_affine_translation(rx, ry);
    m_vxOrg = -Pixels(m_transform.tx);
    m_vyOrg = -Pixels(m_transform.ty);

    //ensure drawer has the new transform, for pixels <-> LUnits conversions
    m_pDrawer->set_transform(m_transform);
}

//---------------------------------------------------------------------------------------
void GraphicView::zoom_out(Pixels x, Pixels y)
{
    LOMSE_LOG_DEBUG(Logger::k_mvc, "");

    double rx(x);
    double ry(y);

    //move origin (left-top window corner) to rx, ry
    m_transform *= agg::trans_affine_translation(-rx, -ry);

    //apply scaling
    m_transform *= agg::trans_affine_scaling(1.0/1.05);

    //move origin back to (rx, ry) so this point remains un-moved
    m_transform *= agg::trans_affine_translation(rx, ry);
    m_vxOrg = -Pixels(m_transform.tx);
    m_vyOrg = -Pixels(m_transform.ty);
}

//---------------------------------------------------------------------------------------
void GraphicView::zoom_fit_full(Pixels screenWidth, Pixels screenHeight)
{
    //move viewport origin (left-top window corner) to top screen, center
    double rx(screenWidth/2);
    double ry(0);
    m_transform *= agg::trans_affine_translation(-rx, -ry);

    //compute required scale
    GraphicModel* pGModel = get_graphic_model();

    //TODO: Dimensions are based on first page. Change this to take measurements
    //from currently displayed page
    GmoBoxDocPage* pPage = pGModel->get_page(0);
    URect rect = pPage->get_bounds();
    double margin = 0.05 * rect.width;      //5% margin, 2.5 at each side
    double xScale = m_pDrawer->Pixels_to_LUnits(screenWidth) / (rect.width + margin);
    double yScale = m_pDrawer->Pixels_to_LUnits(screenHeight) / (rect.height + margin);
    double scale = min (xScale, yScale);

    //apply new user scaling factor
    m_transform.scale(scale);
    m_pDrawer->set_transform(m_transform);

    //move origin back to (rx, ry) so this point remains un-moved
    m_transform *= agg::trans_affine_translation(rx, ry);
    m_vxOrg = -Pixels(m_transform.tx);
    m_vyOrg = -Pixels(m_transform.ty);

    set_viewport_for_page_fit_full(screenWidth);
}

//---------------------------------------------------------------------------------------
void GraphicView::zoom_fit_width(Pixels screenWidth)
{
    //move viewport origin (left-top window corner) to top screen, center
    double rx(screenWidth/2);
    double ry(0);
    m_transform *= agg::trans_affine_translation(-rx, -ry);

    //compute required scale
    GraphicModel* pGModel = get_graphic_model();
    GmoBoxDocPage* pPage = pGModel->get_page(0);
    URect rect = pPage->get_bounds();
    double margin = 0.05 * rect.width;      //5% margin, 2.5 at each side
    double scale = m_pDrawer->Pixels_to_LUnits(screenWidth) / (rect.width + margin);

    //apply new user scaling factor
    m_transform.scale(scale);
    m_pDrawer->set_transform(m_transform);

    //move origin back to (rx, ry) so this point remains un-moved
    m_transform *= agg::trans_affine_translation(rx, ry);
    m_vxOrg = -Pixels(m_transform.tx);
    m_vyOrg = -Pixels(m_transform.ty);

    set_viewport_at_page_center(screenWidth);
}

//---------------------------------------------------------------------------------------
void GraphicView::set_viewport_at_page_center(Pixels screenWidth)
{
    //get page width
    //TODO: Width taken for first page. Change this to use currently displayed page
    GraphicModel* pGModel = get_graphic_model();
    GmoBoxDocPage* pPage = pGModel->get_page(0);
    URect rect = pPage->get_bounds();

    //determine new viewport origin to center page on screen
    Pixels pageWidth = m_pDrawer->LUnits_to_Pixels(rect.width);
    Pixels left = (pageWidth - screenWidth) / 2;

    //force new viewport
    m_vxOrg = left;
    m_transform.tx = double(-left);
}

//---------------------------------------------------------------------------------------
void GraphicView::set_scale(double scale, Pixels x, Pixels y)
{
    double rx(x);
    double ry(y);

    //move origin (left-top window corner) to rx, ry
    m_transform *= agg::trans_affine_translation(-rx, -ry);

    //determine and apply scaling
    double factor = scale / m_transform.scale();
    m_transform *= agg::trans_affine_scaling(factor);

    //move origin back to (rx, ry) so this point remains un-moved
    m_transform *= agg::trans_affine_translation(rx, ry);
    m_vxOrg = -Pixels(m_transform.tx);
    m_vyOrg = -Pixels(m_transform.ty);
}

//---------------------------------------------------------------------------------------
double GraphicView::get_scale()
{
    return m_transform.scale();
}

//---------------------------------------------------------------------------------------
void GraphicView::screen_point_to_page_point(double* x, double* y)
{
    m_pDrawer->screen_point_to_model(x, y);
    int iPage = find_page_at_point(LUnits(*x), LUnits(*y));
    if (iPage != -1)
    {
        URect pageBounds = get_page_bounds(iPage);
        *x -= pageBounds.left();
        *y -= pageBounds.top();
    }
    else
    {
        *x = k_out_of_model;
        *y = k_out_of_model;
    }
}

//---------------------------------------------------------------------------------------
void GraphicView::model_point_to_screen(double* x, double* y, int iPage)
{
    URect pageBounds = get_page_bounds(iPage);
    *x += pageBounds.left();
    *y += pageBounds.top();
    m_pDrawer->model_point_to_screen(x, y);
}

//---------------------------------------------------------------------------------------
UPoint GraphicView::screen_point_to_model_point(Pixels x, Pixels y)
{
    double xm = double(x);
    double ym = double(y);
    m_pDrawer->screen_point_to_model(&xm, &ym);
    return UPoint(xm, ym);
}

//---------------------------------------------------------------------------------------
LUnits GraphicView::pixels_to_lunits(Pixels pixels)
{
    return m_pDrawer->Pixels_to_LUnits(pixels);
}

//---------------------------------------------------------------------------------------
URect GraphicView::get_page_bounds(int iPage)
{
    int i = 0;
    std::list<URect>::iterator it;
    for (it = m_pageBounds.begin(); it != m_pageBounds.end() && i != iPage; ++it, ++i);
    return URect(*it);
}

//---------------------------------------------------------------------------------------
int GraphicView::page_at_screen_point(double x, double y)
{
    //returns -1 if point is out of any page

    double ux = x;
    double uy = y;
    m_pDrawer->screen_point_to_model(&ux, &uy);

    return find_page_at_point(LUnits(ux), LUnits(uy));
}

//---------------------------------------------------------------------------------------
int GraphicView::find_page_at_point(LUnits x, LUnits y)
{
    std::list<URect>::iterator it;
    int iPage = 0;
    for (it = m_pageBounds.begin(); it != m_pageBounds.end(); ++it, ++iPage)
    {
        if ((*it).contains(x, y))
            return iPage;
    }
    return -1;
}

//---------------------------------------------------------------------------------------
void GraphicView::screen_rectangle_to_page_rectangles(Pixels x1, Pixels y1,
                                                      Pixels x2, Pixels y2,
                                                      list<PageRectangle*>* rectangles)
{
    //convert to logical units and normalize rectangle
    double xLeft = double(x1);
    double xRight = double(x2);
    double yTop = double(y1);
    double yBottom = double(y2);
    m_pDrawer->screen_point_to_model(&xLeft, &yTop);
    m_pDrawer->screen_point_to_model(&xRight, &yBottom);

    normalize_rectangle(&xLeft, &yTop, &xRight, &yBottom);

    if (!trim_rectangle_to_be_on_pages(&xLeft, &yTop, &xRight, &yBottom))
        return;     //rectangle out of any page

    trimmed_rectangle_to_page_rectangles(rectangles, xLeft, yTop, xRight, yBottom);
}

//---------------------------------------------------------------------------------------
void GraphicView::normalize_rectangle(double* xLeft, double* yTop,
                                      double* xRight, double* yBottom)
{
    if (*xLeft > *xRight)
    {
        double x = *xLeft;
        *xLeft  = *xRight;
        *xRight = x;
    }

    if (*yTop > *yBottom)
    {
        double y = *yTop;
        *yTop = *yBottom;
        *yBottom = y;
    }
}

//---------------------------------------------------------------------------------------
void GraphicView::trimmed_rectangle_to_page_rectangles(list<PageRectangle*>* rectangles,
                                                       double xLeft, double yTop,
                                                       double xRight, double yBottom)
{
    LUnits xL = LUnits(xLeft);
    LUnits yT = LUnits(yTop);
    LUnits xR = LUnits(xRight);
    LUnits yB = LUnits(yBottom);

    //advance to page containing left-top point
    int page = 0;
    list<URect>::iterator it = m_pageBounds.begin();
    while (it != m_pageBounds.end() && !(*it).contains(xL, yT))
    {
        ++it;
        ++page;
    }

    //loop to add rectangles
    while (it != m_pageBounds.end())
    {
        if (xL < (*it).left())
            xL = (*it).left();
        if (yT < (*it).top())
            yT = (*it).top();
        xR = (LUnits(xRight) > (*it).right() ? (*it).right() : LUnits(xRight));
        yB = (LUnits(yBottom) > (*it).bottom() ? (*it).bottom() : LUnits(yBottom));
        //rectangles are relative to page origin
        rectangles->push_back( LOMSE_NEW PageRectangle(page, xL - (*it).left(),
                                                       yT - (*it).top(),
                                                       xR - (*it).left(),
                                                       yB - (*it).top()) );

        if (xRight <= (*it).right() && LUnits(yBottom) <= (*it).bottom())
            break;

        ++it;
        ++page;
    }
}

//---------------------------------------------------------------------------------------
bool GraphicView::trim_rectangle_to_be_on_pages(double* xLeft, double* yTop,
                                                double* xRight, double* yBottom)
{
    //return true if it has been posible to trim rectangle

    bool fOk = shift_right_x_to_be_on_page(xLeft);
    fOk &= shift_left_x_to_be_on_page(xRight);
    fOk &= shift_down_y_to_be_on_page(yTop);
    fOk &= shift_up_y_to_be_on_page(yBottom);
    return fOk;
}

//---------------------------------------------------------------------------------------
bool GraphicView::shift_right_x_to_be_on_page(double* xLeft)
{
    //returns true if returned x is on a page

    list<URect>::iterator it = m_pageBounds.begin();
    while (it != m_pageBounds.end())
    {
        if (*xLeft < (*it).left() )
        {
            //it is at left of current page. Adjust to this page
            *xLeft = (*it).left();
            return true;
        }
        else if (*xLeft <= (*it).right())
        {
            //it is on this page. Nothing to do
            return true;
        }
        else
        {
            //it is at right of current page. Advance to next page
            ++it;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool GraphicView::shift_left_x_to_be_on_page(double* xRight)
{
    //returns true if returned x is on a page

    list<URect>::reverse_iterator it = m_pageBounds.rbegin();
    while (it != m_pageBounds.rend())
    {
        if (*xRight > (*it).right() )
        {
            //it is at right of current page. Adjust to this page
            *xRight = (*it).right();
            return true;
        }
        else if (*xRight >= (*it).left())
        {
            //it is on this page. Nothing to do
            return true;
        }
        else
        {
            //it is at left of current page. Move to previous page
            ++it;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool GraphicView::shift_down_y_to_be_on_page(double* yTop)
{
    //returns true if returned x is on a page

    list<URect>::iterator it = m_pageBounds.begin();
    while (it != m_pageBounds.end())
    {
        if (*yTop < (*it).top() )
        {
            //it is above of current page. Adjust to this page
            *yTop = (*it).top();
            return true;
        }
        else if (*yTop <= (*it).bottom())
        {
            //it is on this page. Nothing to do
            return true;
        }
        else
        {
            //it is below current page. Advance to next page
            ++it;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------
bool GraphicView::shift_up_y_to_be_on_page(double* yBottom)
{
    //returns true if returned x is on a page

    list<URect>::reverse_iterator it = m_pageBounds.rbegin();
    while (it != m_pageBounds.rend())
    {
        if (*yBottom > (*it).bottom() )
        {
            //it is below current page. Adjust to this page
            *yBottom = (*it).bottom();
            return true;
        }
        else if (*yBottom >= (*it).top())
        {
            //it is on this page. Nothing to do
            return true;
        }
        else
        {
            //it is above current page. Move to previous page
            ++it;
        }
    }
    return false;
}

//---------------------------------------------------------------------------------------
void GraphicView::set_rendering_option(int option, bool value)
{
    switch(option)
    {
        case k_option_draw_box_doc_page_content:
            m_options.draw_box_for(GmoObj::k_box_doc_page_content);
            break;

        case k_option_draw_box_container:
            m_options.draw_box_for(GmoObj::k_box_paragraph);
            m_options.draw_box_for(GmoObj::k_box_score_page);
            m_options.draw_box_for(GmoObj::k_box_table);
            break;

        case k_option_draw_box_system:
            m_options.draw_box_for(GmoObj::k_box_system);
            break;

        case k_option_draw_box_slice:
            m_options.draw_box_for(GmoObj::k_box_slice);
            break;

        case k_option_draw_box_slice_instr:
            m_options.draw_box_for(GmoObj::k_box_slice_instr);
            break;

        case k_option_draw_box_inline_flag:
            m_options.draw_box_for(GmoObj::k_box_inline);
            break;

        case k_option_display_voices_in_colours:
            m_options.draw_voices_coloured = value;
            break;
    }
}

//---------------------------------------------------------------------------------------
void GraphicView::reset_boxes_to_draw()
{
    m_options.reset_boxes_to_draw();
}

//---------------------------------------------------------------------------------------
void GraphicView::set_box_to_draw(int boxType)
{
    m_options.draw_box_for(boxType);
}

//---------------------------------------------------------------------------------------
void GraphicView::highlight_voice(int voice)
{
    m_options.highlighted_voice = voice;
}

//---------------------------------------------------------------------------------------
void GraphicView::change_cursor_voice(int voice)
{
    highlight_voice(voice);
    if (m_pCursor->is_inside_terminal_node()
        && m_pCursor->get_parent_object()->is_score())
    {
        ScoreCursor* pSC = static_cast<ScoreCursor*>(m_pCursor->get_inner_cursor());
        pSC->change_voice_to(voice);
    }
}

//---------------------------------------------------------------------------------------
VSize GraphicView::get_page_size_in_pixels(int nPage)
{
    GraphicModel* pGModel = get_graphic_model();
    GmoBoxDocPage* pPage = pGModel->get_page(nPage-1);
    URect rect = pPage->get_bounds();
    VSize size;
    size.width = m_pDrawer->LUnits_to_Pixels(rect.width);
    size.height = m_pDrawer->LUnits_to_Pixels(rect.height);
    return size;
}

//---------------------------------------------------------------------------------------
void GraphicView::generate_paths()
{
    collect_page_bounds();      //moved out of 'if' block for unit tests
    if (is_valid_viewport())
    {
        int minPage, maxPage;

        determine_visible_pages(&minPage, &maxPage);
        draw_visible_pages(minPage, maxPage);
    }
}

//---------------------------------------------------------------------------------------
void GraphicView::determine_visible_pages(int* minPage, int* maxPage)
{
    list<PageRectangle*> rectangles;
    screen_rectangle_to_page_rectangles(0, 0, m_viewportSize.width,
                                        m_viewportSize.height, &rectangles);

    if (!rectangles.empty())
    {
        *minPage = rectangles.front()->iPage;
        *maxPage = rectangles.back()->iPage;
    }
    else
    {
        *minPage = 0;
        *maxPage = -1;
    }

    delete_rectangles(rectangles);
}

//---------------------------------------------------------------------------------------
void GraphicView::delete_rectangles(list<PageRectangle*>& rectangles)
{
    list<PageRectangle*>::iterator it;
    for (it = rectangles.begin(); it != rectangles.end(); ++it)
        delete *it;
}

//---------------------------------------------------------------------------------------
void GraphicView::draw_visible_pages(int minPage, int maxPage)
{
    GraphicModel* pGModel = get_graphic_model();

    list<URect>::iterator it = m_pageBounds.begin();
    for (int i=0; i < minPage; i++)
        ++it;

    for (int i=minPage; i <= maxPage; i++, ++it)
    {
        UPoint origin = (*it).get_top_left();
        pGModel->draw_page(i, origin, m_pDrawer, m_options);
    }
}

//---------------------------------------------------------------------------------------
UPoint GraphicView::get_page_origin_for(GmoObj* pGmo)
{
    GraphicModel* pGModel = get_graphic_model();
    int iPage = pGModel->get_page_number_containing(pGmo);
    if (iPage < 0)
        return UPoint(0.0, 0.0);

    URect bounds = get_page_bounds(iPage);
    return UPoint(bounds.x, bounds.y);
}

//---------------------------------------------------------------------------------------
void GraphicView::set_rendering_buffer(RenderingBuffer* rbuf)
{
    m_pRenderBuf = rbuf;
    m_pOverlaysGenerator->set_rendering_buffer(rbuf);
}

//---------------------------------------------------------------------------------------
bool GraphicView::is_valid_viewport()
{
    if (m_pRenderBuf)
    {
        m_viewportSize.width = m_pRenderBuf->width();
        m_viewportSize.height = m_pRenderBuf->height();
        return m_viewportSize.width > 0 && m_viewportSize.height > 0;
    }
    return false;
}

////---------------------------------------------------------------------------------------
//void GraphicView::caret_right()
//{
//    m_cursor.move_next();
//}
//
////---------------------------------------------------------------------------------------
//void GraphicView::caret_left()
//{
//    m_cursor.move_prev();
//}
//
////---------------------------------------------------------------------------------------
//void GraphicView::caret_to_object(ImoId nId)
//{
//    m_cursor.reset_and_point_to(nId);
//}
//


//=======================================================================================
// SimpleView implementation
// A graphic view with one page, no margins (i.e. LenMus SscoreAuxCtrol)
//=======================================================================================
SimpleView::SimpleView(LibraryScope& libraryScope, ScreenDrawer* pDrawer)
    : GraphicView(libraryScope, pDrawer)
{
}

//---------------------------------------------------------------------------------------
void SimpleView::collect_page_bounds()
{
    GraphicModel* pGModel = get_graphic_model();
    UPoint origin(0.0f, 0.0f);

    m_pageBounds.clear();
    GmoBoxDocPage* pPage = pGModel->get_page(0);
    URect rect = pPage->get_bounds();
    UPoint bottomRight(origin.x+rect.width, origin.y+rect.height);
    m_pageBounds.push_back( URect(origin, bottomRight) );
}

//---------------------------------------------------------------------------------------
int SimpleView::page_at_screen_point(double UNUSED(x), double UNUSED(y))
{
    return 0;       //simple view is only one page
}


//---------------------------------------------------------------------------------------
void SimpleView::set_viewport_for_page_fit_full(Pixels screenWidth)
{
    set_viewport_at_page_center(screenWidth);
}

//---------------------------------------------------------------------------------------
void SimpleView::get_view_size(Pixels* UNUSED(xWidth), Pixels* UNUSED(yHeight))
{
    //TODO: SimpleView::get_view_size
}


//=======================================================================================
// VerticalBookView implementation
// A graphic view with pages in vertical (i.e. Adobe PDF Reader, MS Word)
//=======================================================================================
VerticalBookView::VerticalBookView(LibraryScope& libraryScope, ScreenDrawer* pDrawer)
    : GraphicView(libraryScope, pDrawer)
{
}

//---------------------------------------------------------------------------------------
void VerticalBookView::collect_page_bounds()
{
    //OPTIMIZATION: this could be computed only once instead of each time the view
    //is repainted.

    GraphicModel* pGModel = get_graphic_model();
    UPoint origin(0.0f, 0.0f);

    m_pageBounds.clear();

    for (int i=0; i < pGModel->get_num_pages(); i++)
    {
        if (i > 0)
            origin.y += 1000;
        GmoBoxDocPage* pPage = pGModel->get_page(i);
        URect rect = pPage->get_bounds();
        UPoint bottomRight(origin.x+rect.width, origin.y+rect.height);
        m_pageBounds.push_back( URect(origin, bottomRight) );
        origin.y += rect.height;
    }
}

//---------------------------------------------------------------------------------------
void VerticalBookView::set_viewport_for_page_fit_full(Pixels screenWidth)
{
    set_viewport_at_page_center(screenWidth);
}

//---------------------------------------------------------------------------------------
void VerticalBookView::get_view_size(Pixels* xWidth, Pixels* yHeight)
{
    LUnits width = 0.0f;
    LUnits height = 0.0f;

    GraphicModel* pGModel = get_graphic_model();
    if (pGModel)
    {
        for (int i=0; i < pGModel->get_num_pages(); i++)
        {
            if (i > 0)
                height += 1000.0f;
            GmoBoxDocPage* pPage = pGModel->get_page(i);
            URect rect = pPage->get_bounds();
            width = max(width, rect.width);
            height += rect.height;
        }
        LUnits margin = 0.05f * width;      //5% margin, 2.5 at each side

        *xWidth = m_pDrawer->LUnits_to_Pixels(width + margin);
        *yHeight = m_pDrawer->LUnits_to_Pixels(height + margin);
    }
}


//=======================================================================================
// HorizontalBookView implementation
// A graphic view with pages in horizontal (i.e. Sibelius)
//=======================================================================================
HorizontalBookView::HorizontalBookView(LibraryScope& libraryScope, ScreenDrawer* pDrawer)
    : GraphicView(libraryScope, pDrawer)
{
}

//---------------------------------------------------------------------------------------
void HorizontalBookView::collect_page_bounds()
{
    //OPTIMIZATION: this could be computed only once instead of each time the view
    //is repainted.

    GraphicModel* pGModel = get_graphic_model();
    UPoint origin(0.0f, 500.0f);

    m_pageBounds.clear();

    for (int i=0; i < pGModel->get_num_pages(); i++)
    {
        GmoBoxDocPage* pPage = pGModel->get_page(i);
        URect rect = pPage->get_bounds();
        UPoint bottomRight(origin.x+rect.width, origin.y+rect.height);
        m_pageBounds.push_back( URect(origin, bottomRight) );
        origin.x += rect.width + 1500;
    }
}

//---------------------------------------------------------------------------------------
void HorizontalBookView::set_viewport_for_page_fit_full(Pixels UNUSED(screenWidth))
{
    //Do nothing, so that vertical position of first displayed page is maintained
//    //TODO: sample image to determine first visible page
//
//    //for page 0 start at 500
//    //for page i start at 500 + (page.width + 1500)*i //each page could have a different width
//    int iCurPage = 1;
//    LUnits left = 500.0f;
//    GraphicModel* pGModel = get_graphic_model();
//    for (int i=0; i < iCurPage; i++)
//    {
//        GmoBoxDocPage* pPage = pGModel->get_page(i);
//        URect rect = pPage->get_bounds();
//        left += rect.width;
//        left += 1500;   //TODO named constant
//    }
//
//    m_vxOrg = - m_pDrawer->LUnits_to_Pixels(left);
//    m_transform.tx = double(m_vxOrg);
}

//---------------------------------------------------------------------------------------
void HorizontalBookView::get_view_size(Pixels* xWidth, Pixels* yHeight)
{
    LUnits width = 500.0f;
    LUnits height = 0.0f;

    GraphicModel* pGModel = get_graphic_model();
    for (int i=0; i < pGModel->get_num_pages(); i++)
    {
        GmoBoxDocPage* pPage = pGModel->get_page(i);
        URect rect = pPage->get_bounds();
        height = max(height, rect.height);
        width += rect.width + 1500;
    }

    *xWidth = m_pDrawer->LUnits_to_Pixels(width);
    *yHeight = m_pDrawer->LUnits_to_Pixels(height);
}


}  //namespace lomse
