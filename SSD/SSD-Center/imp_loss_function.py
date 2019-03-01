import numpy as np

def IoU_loss(localisationsI, glocalisations):
    l_cx = localisationsI[:, :, :, :, 0]
    l_cy = localisationsI[:, :, :, :, 1]
    l_w = localisationsI[:, :, :, :, 2]
    l_h = localisationsI[:, :, :, :, 3]
    g_cx = glocalisations[:, :, :, :, 0]
    g_cy = glocalisations[:, :, :, :, 1]
    g_w = glocalisations[:, :, :, :, 2]
    g_h = glocalisations[:, :, :, :, 3]

    l_S = l_w * l_h
    g_S = g_w * g_h

    l_ymin = l_cx - l_h / 2.
    l_xmin = l_cx - l_w / 2.
    l_ymax = l_cy + l_h / 2.
    l_xmax = l_cx + l_w / 2.
    g_ymin = g_cy - g_h / 2.
    g_xmin = g_cx - g_w / 2.
    g_ymax = g_cy + g_h / 2.
    g_xmax = g_cx + g_w / 2.

    I_w = np.min(l_xmax, g_xmax) - np.max(l_xmin, g_xmin)
    I_h = np.min(l_ymax, g_ymax) - np.max(l_ymin, g_ymin)
    I = I_w * I_h
    U = l_S * g_S - I
    IoU = I / U
    return IoU
