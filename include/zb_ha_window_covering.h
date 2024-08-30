// #ifdef CONFIG_OPENVSH_DEVICE_WINDOW_COVERING

#ifndef ZB_HA_WINDOW_COVERING_H
#define ZB_HA_WINDOW_COVERING_H 1

#define ZB_HA_DEVICE_VER_WINDOW_COVERING 1

#define ZB_HA_WINDOW_COVERING_IN_CLUSTER_NUM 5
#define ZB_HA_WINDOW_COVERING_OUT_CLUSTER_NUM 1

#define ZB_HA_WINDOW_COVERING_REPORT_ATTR_COUNT ZB_ZCL_WINDOW_COVERING_REPORT_ATTR_COUNT

#define ZB_HA_DECLARE_WINDOW_COVERING_CLUSTER_LIST(                          \
    cluster_list_name,                                                       \
    basic_attr_list,                                                         \
    identify_attr_list,                                                      \
    groups_attr_list,                                                        \
    scenes_attr_list,                                                        \
    window_covering_attr_list)                                               \
    zb_zcl_cluster_desc_t cluster_list_name[] =                              \
        {                                                                    \
            ZB_ZCL_CLUSTER_DESC(                                             \
                ZB_ZCL_CLUSTER_ID_BASIC,                                     \
                ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),           \
                (basic_attr_list),                                           \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
                ZB_ZCL_MANUF_CODE_INVALID),                                  \
            ZB_ZCL_CLUSTER_DESC(                                             \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                  \
                ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t),        \
                (identify_attr_list),                                        \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
                ZB_ZCL_MANUF_CODE_INVALID),                                  \
            ZB_ZCL_CLUSTER_DESC(                                             \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                  \
                0,                                                           \
                NULL,                                                        \
                ZB_ZCL_CLUSTER_CLIENT_ROLE,                                  \
                ZB_ZCL_MANUF_CODE_INVALID),                                  \
            ZB_ZCL_CLUSTER_DESC(                                             \
                ZB_ZCL_CLUSTER_ID_GROUPS,                                    \
                ZB_ZCL_ARRAY_SIZE(groups_attr_list, zb_zcl_attr_t),          \
                (groups_attr_list),                                          \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
                ZB_ZCL_MANUF_CODE_INVALID),                                  \
            ZB_ZCL_CLUSTER_DESC(                                             \
                ZB_ZCL_CLUSTER_ID_SCENES,                                    \
                ZB_ZCL_ARRAY_SIZE(scenes_attr_list, zb_zcl_attr_t),          \
                (scenes_attr_list),                                          \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
                ZB_ZCL_MANUF_CODE_INVALID),                                  \
            ZB_ZCL_CLUSTER_DESC(                                             \
                ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,                           \
                ZB_ZCL_ARRAY_SIZE(window_covering_attr_list, zb_zcl_attr_t), \
                (window_covering_attr_list),                                 \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                                  \
                ZB_ZCL_MANUF_CODE_INVALID),                                  \
    }

#define ZB_ZCL_DECLARE_WINDOW_COVERING_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                        \
    ZBOSS_DEVICE_DECLARE_REPORTING_CTX(reporting_info##device_ctx_name,                         \
                                       ZB_HA_WINDOW_COVERING_REPORT_ATTR_COUNT);                \
    ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)                                         \
    simple_desc_##ep_name =                                                                     \
        {                                                                                       \
            ep_id,                                                                              \
            ZB_AF_HA_PROFILE_ID,                                                                \
            ZB_HA_WINDOW_COVERING_DEVICE_ID,                                                    \
            ZB_HA_DEVICE_VER_WINDOW_COVERING,                                                   \
            0,                                                                                  \
            in_clust_num,                                                                       \
            out_clust_num,                                                                      \
            {                                                                                   \
                ZB_ZCL_CLUSTER_ID_BASIC,                                                        \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                     \
                ZB_ZCL_CLUSTER_ID_GROUPS,                                                       \
                ZB_ZCL_CLUSTER_ID_SCENES,                                                       \
                ZB_ZCL_CLUSTER_ID_WINDOW_COVERING,                                              \
            }}

#define ZB_HA_DECLARE_WINDOW_COVERING_EP(ep_name, ep_id, cluster_list)                                                       \
    ZB_ZCL_DECLARE_WINDOW_COVERING_SIMPLE_DESC(ep_name, ep_id,                                                               \
                                               ZB_HA_WINDOW_COVERING_IN_CLUSTER_NUM, ZB_HA_WINDOW_COVERING_OUT_CLUSTER_NUM); \
    ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID, 0, NULL,                                                \
                                ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,                        \
                                (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,                                           \
                                ZB_HA_WINDOW_COVERING_REPORT_ATTR_COUNT, reporting_info##device_ctx_name,                    \
                                0, NULL)

#endif /* ZB_HA_WINDOW_COVERING_H */

// #endif /* CONFIG_OPENVSH_DEVICE_WINDOW_COVERING */
