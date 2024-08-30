#ifdef CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT

#ifndef ZB_HA_ON_OFF_LIGHT_SWITCH_H
#define ZB_HA_ON_OFF_LIGHT_SWITCH_H 1

#define ZB_HA_DEVICE_VER_ON_OFF_LIGHT_SWITCH 1

#define ZB_HA_ON_OFF_LIGHT_SWITCH_IN_CLUSTER_NUM 2
#define ZB_HA_ON_OFF_LIGHT_SWITCH_OUT_CLUSTER_NUM 2

#define ZB_HA_ON_OFF_LIGHT_SWITCH_DEVICE_ID 0x0103

#define ZB_HA_DECLARE_ON_OFF_LIGHT_SWITCH_CLUSTER_LIST(               \
    cluster_list_name,                                                \
    basic_attr_list,                                                  \
    identify_attr_list)                                               \
    zb_zcl_cluster_desc_t cluster_list_name[] =                       \
        {                                                             \
            ZB_ZCL_CLUSTER_DESC(                                      \
                ZB_ZCL_CLUSTER_ID_BASIC,                              \
                ZB_ZCL_ARRAY_SIZE(basic_attr_list, zb_zcl_attr_t),    \
                (basic_attr_list),                                    \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
                ZB_ZCL_MANUF_CODE_INVALID),                           \
            ZB_ZCL_CLUSTER_DESC(                                      \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
                0,                                                    \
                NULL,                                                 \
                ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
                ZB_ZCL_MANUF_CODE_INVALID),                           \
            ZB_ZCL_CLUSTER_DESC(                                      \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                           \
                ZB_ZCL_ARRAY_SIZE(identify_attr_list, zb_zcl_attr_t), \
                (identify_attr_list),                                 \
                ZB_ZCL_CLUSTER_SERVER_ROLE,                           \
                ZB_ZCL_MANUF_CODE_INVALID),                           \
            ZB_ZCL_CLUSTER_DESC(                                      \
                ZB_ZCL_CLUSTER_ID_ON_OFF,                             \
                0,                                                    \
                NULL,                                                 \
                ZB_ZCL_CLUSTER_CLIENT_ROLE,                           \
                ZB_ZCL_MANUF_CODE_INVALID),                           \
    }

#define ZB_ZCL_DECLARE_ON_OFF_LIGHT_SWITCH_SIMPLE_DESC(ep_name, ep_id, in_clust_num, out_clust_num) \
    ZB_DECLARE_SIMPLE_DESC(in_clust_num, out_clust_num);                                            \
    ZB_AF_SIMPLE_DESC_TYPE(in_clust_num, out_clust_num)                                             \
    simple_desc_##ep_name =                                                                         \
        {                                                                                           \
            ep_id,                                                                                  \
            ZB_AF_HA_PROFILE_ID,                                                                    \
            ZB_HA_ON_OFF_LIGHT_SWITCH_DEVICE_ID,                                                    \
            ZB_HA_DEVICE_VER_ON_OFF_LIGHT_SWITCH,                                                   \
            0,                                                                                      \
            in_clust_num,                                                                           \
            out_clust_num,                                                                          \
            {                                                                                       \
                ZB_ZCL_CLUSTER_ID_BASIC,                                                            \
                ZB_ZCL_CLUSTER_ID_IDENTIFY,                                                         \
                ZB_ZCL_CLUSTER_ID_ON_OFF,                                                           \
            }}

#define ZB_HA_DECLARE_ON_OFF_LIGHT_SWITCH_EP(ep_name, ep_id, cluster_list)                                                               \
    ZB_ZCL_DECLARE_ON_OFF_LIGHT_SWITCH_SIMPLE_DESC(ep_name, ep_id,                                                                       \
                                                   ZB_HA_ON_OFF_LIGHT_SWITCH_IN_CLUSTER_NUM, ZB_HA_ON_OFF_LIGHT_SWITCH_OUT_CLUSTER_NUM); \
    ZB_AF_DECLARE_ENDPOINT_DESC(ep_name, ep_id, ZB_AF_HA_PROFILE_ID, 0, NULL,                                                            \
                                ZB_ZCL_ARRAY_SIZE(cluster_list, zb_zcl_cluster_desc_t), cluster_list,                                    \
                                (zb_af_simple_desc_1_1_t *)&simple_desc_##ep_name,                                                       \
                                0, NULL,                                                                                                 \
                                0, NULL)

#endif /* ZB_HA_ON_OFF_LIGHT_SWITCH_H */

#endif /* CONFIG_OPENVSH_DEVICE_ON_OFF_LIGHT */
